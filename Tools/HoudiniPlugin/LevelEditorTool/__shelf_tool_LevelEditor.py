from PySide2 import QtWidgets, QtCore
from functools import partial
import socket
import threading
import hou

# 送信先のIPアドレスとポート番号
serv_address = ('127.0.0.1', 50001)
# UDPソケット
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# フラグで送信状態を管理
is_running = False
is_periodic_sending = False
# スレッド同期のためのロック
lock = threading.Lock()

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
            vertices.append((position.x(), position.y(), position.z()))

        if geo.findVertexAttrib("N"):
            for vertex in geo.iterVertices():
                normal = vertex.attribValue("N")
                normals.append((normal[0], normal[1], normal[2]))
        elif geo.findPointAttrib("N"):
            for point in geo.points():
                normal = point.attribValue("N")
                normals.append((normal[0], normal[1], normal[2]))
        elif geo.findPrimAttrib("N"):
            for prim in geo.prims():
                normal = prim.attribValue("N")
                normals.append((normal[0], normal[1], normal[2]))
        else:
            normals = [(0.0, 0.0, 0.0)] * len(vertices)

        for prim in geo.prims():
            polygons.append([vertex.point().number() + 1 for vertex in prim.vertices()])

        return vertices, normals, polygons
    except AttributeError:
        return [], [], []

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

# パラメータ値を取得する関数
def get_parm_tuple(node, parm_name):
    try:
        return node.evalParmTuple(parm_name)
    except hou.OperationFailed:
        return None

# 親ノードを再帰的にたどり、最終的な絶対変換を計算する関数
def get_absolute_transform_recursive(node, parent_transform):
    transform = calculate_absolute_transform(node, parent_transform)
    parent = node.parent()
    if parent:
        return get_absolute_transform_recursive(parent, transform)
    else:
        return transform

# 終端ノードのジオメトリデータを送信する関数
def send_terminal_node_geometry():
    all_nodes = hou.node("/obj").allSubChildren()
    for node in all_nodes:
        if not isinstance(node, hou.SopNode):
            continue

        if not is_terminal_node(node):
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

        # ノードのトランスフォームを取得
        transform_info = get_absolute_transform_recursive(node, {
            "Translate": (0, 0, 0),
            "Rotate": (0, 0, 0),
            "Scale": (1, 1, 1)
        })

        # 頂点データをワールド座標系に変換
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

        try:
            sock.sendto(obj_data.encode('utf-8'), serv_address)
            print(f"送信しました:\n{obj_data}")
        except Exception as e:
            print(f"送信エラー: {e}")

# 通信の開始と停止
def start_sending():
    global is_running
    if is_running:
        print("既に通信が開始されています。")
        return
    with lock:
        is_running = True
    print("通信スレッドを開始しました。")

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

# 定期的送信の停止
def stop_periodic_sending():
    global is_periodic_sending
    if not is_periodic_sending:
        print("定期送信は既に停止しています。")
        return
    
    is_periodic_sending = False
    periodic_timer.stop()
    print("定期送信停止を要求しました。")

# ダイアログの作成
dialog = QtWidgets.QWidget()
dialog.setWindowTitle("レベルエディタ(仮)")
dialog.setGeometry(100, 100, 500, 300)
dialog.setParent(hou.qt.mainWindow(), QtCore.Qt.Window)

# レイアウト
v_layout = QtWidgets.QVBoxLayout()

# テキストエディタ
text_editor = QtWidgets.QLineEdit()

# ボタン作成
send_button = QtWidgets.QPushButton("テキストを送信")
send_button.clicked.connect(partial(send_message, text_editor))

sendTerminalData_button = QtWidgets.QPushButton("終端ノードデータを送信")
sendTerminalData_button.clicked.connect(send_terminal_node_geometry)

start_button = QtWidgets.QPushButton("通信開始")
start_button.clicked.connect(start_sending)

stop_button = QtWidgets.QPushButton("通信停止")
stop_button.clicked.connect(stop_sending)

# 定期送信関連
periodic_interval_input = QtWidgets.QLineEdit()
periodic_interval_input.setPlaceholderText("送信間隔（秒）")

start_periodic_button = QtWidgets.QPushButton("定期送信開始")
start_periodic_button.clicked.connect(start_periodic_sending)

stop_periodic_button = QtWidgets.QPushButton("定期送信停止")
stop_periodic_button.clicked.connect(stop_periodic_sending)

# レイアウトにウィジェットを追加
v_layout.addWidget(text_editor)
v_layout.addWidget(send_button)
v_layout.addWidget(sendTerminalData_button)
v_layout.addWidget(start_button)
v_layout.addWidget(stop_button)

# 定期送信関連のウィジェット追加
v_layout.addWidget(periodic_interval_input)
v_layout.addWidget(start_periodic_button)
v_layout.addWidget(stop_periodic_button)

dialog.setLayout(v_layout)
dialog.show()

# 定期送信タイマー
periodic_timer = QtCore.QTimer()
periodic_timer.timeout.connect(send_terminal_node_geometry)