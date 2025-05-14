from PySide2 import QtWidgets, QtCore
from functools import partial
import zlib
import socket
import threading
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
dialog.resize(500, 400)  # 少し余裕を持たせる
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
meshsync_layout.addLayout(periodic_layout)
meshsync_layout.addStretch()


# ========== Test タブ ==========
test_tab = QtWidgets.QWidget()
test_layout = QtWidgets.QVBoxLayout()
test_layout.setContentsMargins(10, 10, 10, 10)
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
checkbox = QtWidgets.QCheckBox("同意します")

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
tab_widget.addTab(test_tab, "Test")
tab_widget.addTab(none_tab, "None")

main_layout.addWidget(tab_widget)
dialog.setLayout(main_layout)
dialog.show()

# --- 定期送信タイマー ---
periodic_timer = QtCore.QTimer()
periodic_timer.timeout.connect(send_terminal_node_geometry)
