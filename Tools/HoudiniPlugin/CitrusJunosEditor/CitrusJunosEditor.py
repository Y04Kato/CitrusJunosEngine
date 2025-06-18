from PySide2 import QtWidgets, QtCore
from functools import partial
import zlib
import socket
import threading
import json
import os
import hou

# 送信先のIPアドレスとポート番号
serv_address = ('127.0.0.1', 50001)
# UDPソケット
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# APIの最大データサイズ
MAX_PACKET_SIZE = 32768

# フラグで送信状態を管理
is_running = False
is_periodic_sending = False
# スレッド同期のためのロック
lock = threading.Lock()

################################
# チェックボックスの状態
send_only_leoutput = False
# チェックボックスの変更を検知する関数
def toggle_send_only_leoutput(state):
    global send_only_leoutput
    send_only_leoutput = state == QtCore.Qt.Checked

# ノードが終端ノードかどうかを判定する関数
def is_terminal_node(node):
    return len(node.outputs()) == 0

# ジオメトリ情報を取得する関数
def get_geometry_info(node):
    try:
        geo = node.geometry()
        vertices = []
        normals = []
        polygons = []

        for point in geo.points():
            position = point.position()
            vertices.append((-position.x(), position.y(), position.z()))

        if geo.findVertexAttrib("N"):
            for vertex in geo.iterVertices():
                normal = vertex.attribValue("N")
                normals.append((-normal[0], normal[1], normal[2]))
        elif geo.findPointAttrib("N"):
            for point in geo.points():
                normal = point.attribValue("N")
                normals.append((-normal[0], normal[1], normal[2]))
        elif geo.findPrimAttrib("N"):
            for prim in geo.prims():
                normal = prim.attribValue("N")
                normals.append((-normal[0], normal[1], normal[2]))
        else:
            normals = [(0.0, 0.0, 0.0)] * len(vertices)

        for prim in geo.prims():
            polygons.append([vertex.point().number() + 1 for vertex in prim.vertices()])

        return vertices, normals, polygons
    except AttributeError:
        return [], [], []

# パラメータ値を取得する関数
def get_parm_tuple(node, parm_name):
    try:
        return node.evalParmTuple(parm_name)
    except hou.OperationFailed:
        return None

# Transform情報を計算する関数（親ノードの座標を合成）
def calculate_absolute_transform(node, parent_transform):
    translate = get_parm_tuple(node, "t")
    absolute_translate = tuple(
        (translate[i] if translate else 0) + parent_transform["Translate"][i]
        for i in range(3)
    )
    rotate = get_parm_tuple(node, "r")
    absolute_rotate = tuple(
        (rotate[i] if rotate else 0) + parent_transform["Rotate"][i]
        for i in range(3)
    )
    scale = get_parm_tuple(node, "s")
    absolute_scale = tuple(
        (scale[i] if scale else 1) * parent_transform["Scale"][i]
        for i in range(3)
    )

    # Editノードの場合、親のトランスフォームのみ継承
    if node.type().name() == "edit":
        return parent_transform

    return {
        "Translate": absolute_translate,
        "Rotate": absolute_rotate,
        "Scale": absolute_scale
    }

# 親ノードを再帰的にたどり、最終的な絶対変換を計算する関数
def get_absolute_transform_recursive(node, parent_transform):
    transform = calculate_absolute_transform(node, parent_transform)
    parent = node.parent()
    if parent:
        return get_absolute_transform_recursive(parent, transform)
    else:
        return transform

# 送信データサイズが大きすぎるときに分割して送信する関数
def send_data_in_chunks(compressed_data):
    """データが API の限界を超える場合、分割して送信"""
    total_size = len(compressed_data)
    offset = 0

    while offset < total_size:
        # 送信するデータのサイズを決定（MAX_PACKET_SIZE 以下）
        chunk = compressed_data[offset:offset + MAX_PACKET_SIZE]

        # 最後のデータには `/add` を追加しない
        if offset + MAX_PACKET_SIZE < total_size:
            chunk += b"/add"

        try:
            sock.sendto(chunk, serv_address)
            print(f"送信: {len(chunk)} バイト")
        except Exception as e:
            print(f"送信エラー: {e}")

        offset += MAX_PACKET_SIZE

# 終端ノードのジオメトリデータを送信する関数
def send_terminal_node_geometry():
    """終端ノードのジオメトリデータを取得し、分割送信"""
    all_nodes = hou.node("/obj").allSubChildren()
    
    for node in all_nodes:
        if not isinstance(node, hou.SopNode):
            continue
        if not is_terminal_node(node):
            continue
        if send_only_leoutput and node.name() != "LEOutput":
            continue

        try:
            geo = node.geometry()
        except hou.GeometryPermissionError:
            print(f"ノード {node.path()} のジオメトリにアクセスできません。")
            continue
        except AttributeError:
            print(f"ノード {node.path()} はジオメトリを持っていません。")
            continue

        vertices, normals, polygons = get_geometry_info(node)
        if not vertices:
            print(f"ノード {node.path()} の頂点データが空です。")
            continue

        full_name = node.path()
        transform_info = get_absolute_transform_recursive(node, {
            "Translate": (0, 0, 0),
            "Rotate": (0, 0, 0),
            "Scale": (1, 1, 1)
        })

        transformed_vertices = [
            (
                v[0] * transform_info["Scale"][0] + transform_info["Translate"][0],
                v[1] * transform_info["Scale"][1] + transform_info["Translate"][1],
                v[2] * transform_info["Scale"][2] + transform_info["Translate"][2],
            )
            for v in vertices
        ]

        obj_data = f"Name: {full_name}\n"
        obj_data += (
            f"Translate {transform_info['Translate'][0]:.3f} "
            f"{transform_info['Translate'][1]:.3f} "
            f"{-transform_info['Translate'][2]:.3f}\n"
            f"Rotate {transform_info['Rotate'][0]:.3f} "
            f"{transform_info['Rotate'][1]:.3f} "
            f"{transform_info['Rotate'][2]:.3f}\n"
            f"Scale {transform_info['Scale'][0]:.3f} "
            f"{transform_info['Scale'][1]:.3f} "
            f"{transform_info['Scale'][2]:.3f}\n"
        )
        obj_data += "\n".join(f"v {v[0]:.3f} {v[1]:.3f} {v[2]:.3f}" for v in transformed_vertices) + "\n"
        obj_data += "\n".join(f"vn {n[0]:.3f} {n[1]:.3f} {n[2]:.3f}" for n in normals) + "\n"
        obj_data += "\n".join("f " + " ".join(map(str, face)) for face in polygons)

        # データを圧縮
        compressed_data = zlib.compress(obj_data.encode('utf-8'))

        # 分割送信
        send_data_in_chunks(compressed_data)

# ジオメトリデータをJSONとして保存
def save_terminal_node_geometry_to_json():
    all_nodes = hou.node("/obj").allSubChildren()
    data_list = []

    for node in all_nodes:
        if not isinstance(node, hou.SopNode):
            continue
        if not is_terminal_node(node):
            continue
        if send_only_leoutput and node.name() != "LEOutput":
            continue

        try:
            geo = node.geometry()
        except (hou.GeometryPermissionError, AttributeError):
            continue

        vertices, normals, polygons = get_geometry_info(node)
        if not vertices:
            continue

        transform_info = get_absolute_transform_recursive(node, {
            "Translate": (0, 0, 0),
            "Rotate": (0, 0, 0),
            "Scale": (1, 1, 1)
        })

        transformed_vertices = [
            (
                v[0] * transform_info["Scale"][0] + transform_info["Translate"][0],
                v[1] * transform_info["Scale"][1] + transform_info["Translate"][1],
                v[2] * transform_info["Scale"][2] + transform_info["Translate"][2],
            )
            for v in vertices
        ]

        data = {
            "name": node.path(),
            "transform": {
                "translate": transform_info["Translate"],
                "rotate": transform_info["Rotate"],
                "scale": transform_info["Scale"]
            },
            "vertices": transformed_vertices,
            "normals": normals,
            "polygons": polygons
        }

        data_list.append(data)

    if not data_list:
        print("保存対象のジオメトリが見つかりませんでした。")
        return

    file_path = QtWidgets.QFileDialog.getSaveFileName(
        dialog, "JSONファイルとして保存", "", "JSON Files (*.json)"
    )[0]

    if not file_path:
        print("保存がキャンセルされました。")
        return

    try:
        with open(file_path, 'w', encoding='utf-8') as f:
            json.dump(data_list, f, ensure_ascii=False, indent=2)
        print(f"ジオメトリデータを保存しました: {file_path}")
    except Exception as e:
        print(f"保存中にエラーが発生しました: {e}")

# 通信の開始
def start_sending():
    global is_running
    if is_running:
        print("既に通信が開始されています。")
        return
    with lock:
        is_running = True
    print("通信スレッドを開始しました。")

# 通信の停止
def stop_sending():
    global is_running
    if not is_running:
        print("通信は既に停止しています。")
        return
    with lock:
        is_running = False
    print("通信停止を要求しました。")

# 定期的送信の開始
def start_periodic_sending():
    global is_periodic_sending
    if is_periodic_sending:
        print("既に定期送信が開始されています。")
        return
    
    interval = int(periodic_interval_input.text()) * 1000  # 秒をミリ秒に変換
    if interval <= 0:
        print("無効な秒数です。")
        return
    
    is_periodic_sending = True
    periodic_timer.start(interval)
    print(f"{interval / 1000} 秒ごとに送信を開始しました。")

# 定期送信の停止
def stop_periodic_sending():
    global is_periodic_sending
    if not is_periodic_sending:
        print("定期送信は既に停止しています。")
        return
    
    is_periodic_sending = False
    periodic_timer.stop()
    print("定期送信停止を要求しました。")

# 単一モデルとしてOBJ出力する
def export_all_terminal_geometry_as_obj():
    def is_terminal_node(node):
        return not node.outputs()

    def get_geometry_info(node):
        geo = node.geometry()
        vertices = [point.position() for point in geo.points()]
        polygons = []

        for prim in geo.prims():
            if prim.type() == hou.primType.Polygon:
                polygons.append([v.point().number() for v in prim.vertices()])

        return vertices, polygons

    def get_absolute_transform_recursive(node, acc=None):
        if acc is None:
            acc = {
                "Translate": (0, 0, 0),
                "Rotate": (0, 0, 0),
                "Scale": (1, 1, 1)
            }

        if isinstance(node, hou.SopNode):
            node = node.parent()

        if node.path() == "/obj" or node.parent() is None:
            return acc

        t = node.parmTuple("t").eval() if node.parmTuple("t") else (0, 0, 0)
        s = node.parmTuple("s").eval() if node.parmTuple("s") else (1, 1, 1)

        acc["Translate"] = tuple(acc["Translate"][i] + t[i] for i in range(3))
        acc["Scale"] = tuple(acc["Scale"][i] * s[i] for i in range(3))

        return get_absolute_transform_recursive(node.parent(), acc)

    send_only_leoutput = True

    all_nodes = hou.node("/obj").allSubChildren()
    all_vertices = []
    all_faces = []
    vertex_offset = 0

    for node in all_nodes:
        if not isinstance(node, hou.SopNode):
            continue
        if not is_terminal_node(node):
            continue
        if send_only_leoutput and node.name() != "LEOutput":
            continue

        try:
            vertices, polygons = get_geometry_info(node)
            if not vertices:
                continue

            transform = get_absolute_transform_recursive(node)

            transformed_vertices = [
                (
                    v[0] * transform["Scale"][0] + transform["Translate"][0],
                    v[1] * transform["Scale"][1] + transform["Translate"][1],
                    v[2] * transform["Scale"][2] + transform["Translate"][2]
                )
                for v in vertices
            ]

            all_vertices.extend(transformed_vertices)

            for face in polygons:
                corrected_face = [idx + 1 + vertex_offset for idx in face]
                all_faces.append(corrected_face)

            vertex_offset += len(vertices)

        except Exception as e:
            print(f"[エラー] ノード {node.path()} の処理中にエラー発生: {e}")
            import traceback
            traceback.print_exc()

    if not all_vertices:
        print("エクスポート可能なジオメトリが見つかりませんでした。")
        return

    save_path, _ = QtWidgets.QFileDialog.getSaveFileName(None, "OBJとして保存", "", "Wavefront OBJ (*.obj)")
    if not save_path:
        return

    with open(save_path, "w", encoding="utf-8") as f:
        for v in all_vertices:
            f.write(f"v {v[0]:.6f} {v[1]:.6f} {v[2]:.6f}\n")
        for face in all_faces:
            f.write("f " + " ".join(str(idx) for idx in face) + "\n")

    print(f"エクスポート完了: {save_path}")

################################

# パラメータを抜粋して管理する
class ParameterSyncTool(QtWidgets.QWidget):
    def __init__(self, parent=None):
        super(ParameterSyncTool, self).__init__(parent)
        self.synced_parms = {}  # {node_path: {parm_name: {...}}}
        self.node_blocks = {}   # node_path -> block_widget

        layout = QtWidgets.QVBoxLayout(self)

        add_btn = QtWidgets.QPushButton("選択ノードを追加")
        add_btn.clicked.connect(self.add_selected_node)
        layout.addWidget(add_btn)

        self.scroll = QtWidgets.QScrollArea()
        self.scroll.setWidgetResizable(True)
        self.scroll_widget = QtWidgets.QWidget()
        self.scroll_layout = QtWidgets.QVBoxLayout(self.scroll_widget)
        self.scroll_layout.setSpacing(8)
        self.scroll_layout.addStretch(1)
        self.scroll.setWidget(self.scroll_widget)
        layout.addWidget(self.scroll)

    def add_selected_node(self):
        for node in hou.selectedNodes():
            if node.path() not in self.synced_parms:
                self.add_node_block(node)

    def add_node_block(self, node):
        node_path = node.path()
        self.synced_parms[node_path] = {}
        self.node_blocks[node_path] = {}

        outer_widget = QtWidgets.QWidget()
        outer_layout = QtWidgets.QVBoxLayout(outer_widget)
        outer_layout.setContentsMargins(4, 4, 4, 4)

        header_layout = QtWidgets.QHBoxLayout()
        toggle_btn = QtWidgets.QToolButton()
        toggle_btn.setText("▼")
        toggle_btn.setCheckable(True)
        toggle_btn.setChecked(True)

        label = QtWidgets.QLabel(f"{node.name()}  ({node_path})")
        label.setStyleSheet("font-weight: bold;")
        remove_btn = QtWidgets.QPushButton("同期解除")
        remove_btn.setFixedWidth(90)
        show_all_btn = QtWidgets.QPushButton("全パラメータ表示")
        show_all_btn.setFixedWidth(120)

        header_layout.addWidget(toggle_btn)
        header_layout.addWidget(label)
        header_layout.addStretch()
        header_layout.addWidget(show_all_btn)
        header_layout.addWidget(remove_btn)

        outer_layout.addLayout(header_layout)

        content_widget = QtWidgets.QWidget()
        content_layout = QtWidgets.QVBoxLayout(content_widget)
        content_layout.setContentsMargins(10, 4, 4, 4)
        content_layout.setSpacing(6)
        outer_layout.addWidget(content_widget)

        toggle_btn.toggled.connect(lambda checked: content_widget.setVisible(checked))
        toggle_btn.toggled.connect(lambda checked: toggle_btn.setText("▼" if checked else "▶"))

        selected_label = QtWidgets.QLabel("追加済みパラメータ:")
        selected_layout = QtWidgets.QVBoxLayout()
        selected_layout.setSpacing(4)
        selected_widget = QtWidgets.QWidget()
        selected_widget.setLayout(selected_layout)
        content_layout.addWidget(selected_label)
        content_layout.addWidget(selected_widget)

        def remove_node_block():
            for data in self.synced_parms[node_path].values():
                data['timer'].stop()
                data['container'].deleteLater()
            del self.synced_parms[node_path]
            self.scroll_layout.removeWidget(outer_widget)
            outer_widget.deleteLater()

        def show_all_parms_window():
            dialog = QtWidgets.QDialog(self)
            dialog.setWindowTitle(f"{node.name()} の全パラメータ")
            dialog.setMinimumWidth(400)
            vbox = QtWidgets.QVBoxLayout(dialog)

            filter_input = QtWidgets.QLineEdit()
            filter_input.setPlaceholderText("パラメータ名やラベルでフィルタ")
            vbox.addWidget(filter_input)

            scroll = QtWidgets.QScrollArea()
            scroll.setWidgetResizable(True)
            inner_widget = QtWidgets.QWidget()
            inner_layout = QtWidgets.QVBoxLayout(inner_widget)
            scroll.setWidget(inner_widget)
            vbox.addWidget(scroll)

            def refresh():
                for i in reversed(range(inner_layout.count())):
                    w = inner_layout.itemAt(i).widget()
                    if w: w.setParent(None)
                    
                keyword = filter_input.text().lower()

                already_seen = set()
                
                for parm in node.parms():
                    ptuple = parm.tuple()
                    if not ptuple:
                        continue

                    if parm != ptuple[0]:
                        continue  # tupleの代表（txなど）以外はスキップ

                    name = parm.name()
                    label = parm.parmTemplate().label()
                    
                    if name in already_seen:
                        continue
                    already_seen.add(name)

                    row = QtWidgets.QHBoxLayout()
                    row.setSpacing(5)
                    display_name = label or name
                    row.addWidget(QtWidgets.QLabel(display_name))
                    add_btn = QtWidgets.QPushButton("追加")
                    add_btn.setFixedWidth(60)
                    add_btn.clicked.connect(partial(self.add_synced_parm, node, name, selected_layout))
                    row.addWidget(add_btn)

                    container = QtWidgets.QWidget()
                    container.setLayout(row)
                    inner_layout.addWidget(container)

            filter_input.textChanged.connect(refresh)
            refresh()

            dialog.exec_()

        remove_btn.clicked.connect(remove_node_block)
        show_all_btn.clicked.connect(show_all_parms_window)

        self.scroll_layout.insertWidget(self.scroll_layout.count() - 1, outer_widget)
        self.node_blocks[node_path]['selected_layout'] = selected_layout

    def add_synced_parm(self, node, parm_name, parent_layout):
        node_path = node.path()
        if parm_name in self.synced_parms[node_path]:
            return

        parm = node.parm(parm_name)
        if not parm:
            return

        ptemplate = parm.parmTemplate()
        ptype = ptemplate.type()
        widgets = []
        timer = QtCore.QTimer(self)

        row = QtWidgets.QHBoxLayout()
        row.setSpacing(5)
        row.setContentsMargins(2, 2, 2, 2)

        display_label = ptemplate.label() or parm_name
        label = QtWidgets.QLabel(display_label)
        label.setFixedWidth(100)
        remove_btn = QtWidgets.QPushButton("✕")
        remove_btn.setFixedWidth(40)

        row.addWidget(label)

        def safe_eval(p):
            try:
                return p.eval()
            except:
                return None

        def create_ui_widget():
            if ptype == hou.parmTemplateType.Button:
                btn = QtWidgets.QPushButton("実行")
                btn.clicked.connect(lambda: parm.pressButton())
                return [btn]

            elif ptype == hou.parmTemplateType.Toggle:
                cb = QtWidgets.QCheckBox()
                cb.setChecked(bool(safe_eval(parm)))
                def update_ui(): cb.setChecked(bool(safe_eval(parm)))
                def update_parm(): parm.set(int(cb.isChecked()))
                cb.stateChanged.connect(update_parm)
                timer.timeout.connect(update_ui)
                return [cb]

            elif ptype == hou.parmTemplateType.Menu:
                combo = QtWidgets.QComboBox()
                items = ptemplate.menuItems()
                labels = ptemplate.menuLabels()
                combo.addItems(labels or items)
                def update_ui():
                    val = parm.evalAsString()
                    if val in items:
                        index = items.index(val)
                        if combo.currentIndex() != index:
                            combo.setCurrentIndex(index)
                def update_parm(index): parm.set(items[index])
                combo.currentIndexChanged.connect(update_parm)
                timer.timeout.connect(update_ui)
                return [combo]

            elif ptype in [hou.parmTemplateType.Int, hou.parmTemplateType.Float]:
                    try:
                        parms = parm.tuple()
                    except Exception:
                        parms = [parm]
                
                    widget_list = []
                    for p in parms:
                        if p is None:
                            continue

                        pt = p.parmTemplate()
                        has_slider = pt.minIsStrict() or pt.maxIsStrict()  # スライダーっぽさの推定

                        edit = QtWidgets.QLineEdit(str(safe_eval(p) or 0))
                        edit.setFixedWidth(150)

                        slider = None
                        if has_slider:
                            slider = QtWidgets.QSlider(QtCore.Qt.Horizontal)
                            slider.setFixedWidth(150)
                            min_val = pt.minValue()
                            max_val = pt.maxValue()

                            # 安全確認：最小と最大が等しい場合は範囲に幅を持たせる
                            if min_val == max_val:
                                min_val -= 1
                                max_val += 1

                            # スライダーは整数範囲なので適当な倍率を掛ける（例: 100倍）
                            slider.setRange(int(min_val * 100), int(max_val * 100))

                        def update_ui(e=edit, par=p, s=slider):
                            if not e.hasFocus():
                                val = safe_eval(par)
                                if val is not None:
                                    e.setText(str(val))
                                    if s:
                                        s.setValue(int(val * 100))

                        def update_parm(e=edit, par=p, s=slider):
                            try:
                                val = float(e.text())
                                if pt == hou.parmTemplateType.Int:
                                            par.set(int(round(val)))
                                else:
                                    par.set(val)
                                if s:
                                    s.setValue(int(val * 100))
                            except:
                                pass

                        def update_slider(val, par=p, e=edit):
                            pt = par.parmTemplate().type()
                            if pt == hou.parmTemplateType.Int:
                                int_val = int(round(val / 100.0))
                                par.set(int_val)
                            else:
                                par.set(val / 100.0)
                            e.setText(str(par.eval()))

                        edit.editingFinished.connect(update_parm)
                        if slider:
                            slider.valueChanged.connect(update_slider)

                        timer.timeout.connect(update_ui)

                        if slider:
                            widget_list.append((edit, slider))
                        else:
                            widget_list.append((edit,))

                    # 横並びウィジェット作成
                    container = QtWidgets.QWidget()
                    h_layout = QtWidgets.QHBoxLayout(container)
                    h_layout.setSpacing(5)
                    h_layout.setContentsMargins(0,0,0,0)
                    for widgets in widget_list:
                        for w in widgets:
                            h_layout.addWidget(w)
                    return [container]

            elif ptype == hou.parmTemplateType.String:
                le = QtWidgets.QLineEdit(str(parm.evalAsString()))
                def update_ui():
                    if not le.hasFocus():
                        le.setText(str(parm.evalAsString()))
                def update_parm():
                    parm.set(le.text())
                le.editingFinished.connect(update_parm)
                timer.timeout.connect(update_ui)
                return [le]

            else:
                # 未対応タイプは表示のみ
                label = QtWidgets.QLabel("(未対応)")
                return [label]

        widgets_ui = create_ui_widget()
        for w in widgets_ui:
            row.addWidget(w)

        row.addWidget(remove_btn)
        container = QtWidgets.QWidget()
        container.setLayout(row)
        parent_layout.addWidget(container)

        def remove():
            timer.stop()
            parent_layout.removeWidget(container)
            container.deleteLater()
            del self.synced_parms[node_path][parm_name]

        remove_btn.clicked.connect(remove)

        if ptype != hou.parmTemplateType.Button:
            timer.setInterval(500)
            timer.start()

        self.synced_parms[node_path][parm_name] = {
            'widget': widgets_ui,
            'timer': timer,
            'container': container
        }
    
################################
# テキストメッセージ送信関数
def send_message(message):
    message = message.text()
    if message:
        try:
            sock.sendto(message.encode('utf-8'), serv_address)
            print(f"送信しました: {message}")
        except Exception as e:
            print(f"送信エラー: {e}")
    else:
        print("メッセージが空です。")

################################


################################

# --- ダイアログ設定 ---
dialog = QtWidgets.QWidget()
dialog.setWindowTitle("CitrusJunosEditor")
dialog.resize(700, 400)
dialog.setParent(hou.qt.mainWindow(), QtCore.Qt.Window)

# --- メインレイアウト ---
main_layout = QtWidgets.QVBoxLayout()
main_layout.setContentsMargins(12, 12, 12, 12)
main_layout.setSpacing(10)

# --- タブウィジェット ---
tab_widget = QtWidgets.QTabWidget()

# ========== MeshSync タブ ==========
meshsync_tab = QtWidgets.QWidget()
meshsync_layout = QtWidgets.QVBoxLayout()
meshsync_layout.setContentsMargins(10, 10, 10, 10)
meshsync_layout.setSpacing(8)
meshsync_tab.setLayout(meshsync_layout)

# --- チェックボックス ---
leoutput_checkbox = QtWidgets.QCheckBox("LEOutputのみ送信")
leoutput_checkbox.stateChanged.connect(toggle_send_only_leoutput)

# --- 通信ボタン（横並び） ---
comm_button_layout = QtWidgets.QHBoxLayout()
start_button = QtWidgets.QPushButton("通信開始")
stop_button = QtWidgets.QPushButton("通信停止")
start_button.clicked.connect(start_sending)
stop_button.clicked.connect(stop_sending)
comm_button_layout.addWidget(start_button)
comm_button_layout.addWidget(stop_button)

# --- 終端ノード送信 ---
sendTerminalData_button = QtWidgets.QPushButton("終端ノードデータを送信")
sendTerminalData_button.clicked.connect(send_terminal_node_geometry)

# --- JSON出力 ---
save_json_button = QtWidgets.QPushButton("終端ノードデータをJSON保存")
save_json_button.clicked.connect(save_terminal_node_geometry_to_json)

# --- OBJエクスポート ---
export_obj_button = QtWidgets.QPushButton(".objとしてエクスポート（単一モデル化）")
export_obj_button.clicked.connect(export_all_terminal_geometry_as_obj)

# --- 定期送信（横並び） ---
periodic_layout = QtWidgets.QHBoxLayout()
periodic_interval_input = QtWidgets.QLineEdit()
periodic_interval_input.setPlaceholderText("送信間隔（秒）")
periodic_interval_input.setFixedWidth(100)

start_periodic_button = QtWidgets.QPushButton("定期送信開始")
start_periodic_button.clicked.connect(start_periodic_sending)
stop_periodic_button = QtWidgets.QPushButton("定期送信停止")
stop_periodic_button.clicked.connect(stop_periodic_sending)

periodic_layout.addWidget(periodic_interval_input)
periodic_layout.addWidget(start_periodic_button)
periodic_layout.addWidget(stop_periodic_button)

# --- MeshSyncレイアウトに追加 ---
meshsync_layout.addWidget(leoutput_checkbox)
meshsync_layout.addLayout(comm_button_layout)
meshsync_layout.addWidget(sendTerminalData_button)
meshsync_layout.addWidget(save_json_button)
meshsync_layout.addWidget(export_obj_button)
meshsync_layout.addLayout(periodic_layout)
meshsync_layout.addStretch()

# ========== ParameterSync タブ ==========
sync_tool_tab = ParameterSyncTool()

# ========== Test タブ ==========
test_tab = QtWidgets.QWidget()
test_layout = QtWidgets.QVBoxLayout()
test_layout.setContentsMargins(12, 12, 12, 12)
test_layout.setSpacing(10)
test_tab.setLayout(test_layout)

# --- テキスト入力と送信（横並び） ---
text_input_layout = QtWidgets.QHBoxLayout()
test_texteditor = QtWidgets.QLineEdit()
test_sendbutton = QtWidgets.QPushButton("テキストを送信")
test_sendbutton.clicked.connect(partial(send_message, test_texteditor))
text_input_layout.addWidget(test_texteditor)
text_input_layout.addWidget(test_sendbutton)

# --- チェックボックス ---
checkbox = QtWidgets.QCheckBox("チェックボックス")

# --- ラジオボタン ---
radio_group_box = QtWidgets.QGroupBox("オプション選択")
radio_layout = QtWidgets.QVBoxLayout()
for text in ["オプション A", "オプション B", "オプション C"]:
    btn = QtWidgets.QRadioButton(text)
    radio_layout.addWidget(btn)
radio_layout.itemAt(0).widget().setChecked(True)
radio_group_box.setLayout(radio_layout)

# --- スライダーとプログレスバー ---
slider = QtWidgets.QSlider(QtCore.Qt.Horizontal)
slider.setRange(0, 100)
progress = QtWidgets.QProgressBar()
progress.setRange(0, 100)
slider.valueChanged.connect(progress.setValue)

# --- コンボボックス ---
combo_row = QtWidgets.QHBoxLayout()
combo_label = QtWidgets.QLabel("選択してください:")
combo_box = QtWidgets.QComboBox()
combo_box.addItems(["選択肢 1", "選択肢 2", "選択肢 3"])
combo_row.addWidget(combo_label)
combo_row.addWidget(combo_box)

# --- Testタブに追加 ---
test_layout.addLayout(text_input_layout)
test_layout.addWidget(checkbox)
test_layout.addWidget(radio_group_box)
test_layout.addWidget(slider)
test_layout.addWidget(progress)
test_layout.addLayout(combo_row)
test_layout.addStretch()


# ========== None タブ ==========
none_tab = QtWidgets.QWidget()
none_layout = QtWidgets.QVBoxLayout()
none_tab.setLayout(none_layout)


# --- タブ追加 ---
tab_widget.addTab(meshsync_tab, "MeshSync")
tab_widget.addTab(sync_tool_tab, "SyncTool")
tab_widget.addTab(test_tab, "Test")
tab_widget.addTab(none_tab, "None")

main_layout.addWidget(tab_widget)
dialog.setLayout(main_layout)
dialog.show()

# --- 定期送信タイマー ---
periodic_timer = QtCore.QTimer()
periodic_timer.timeout.connect(send_terminal_node_geometry)