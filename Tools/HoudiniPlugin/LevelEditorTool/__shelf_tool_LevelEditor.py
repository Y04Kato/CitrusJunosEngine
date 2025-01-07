from PySide2 import QtWidgets, QtCore
from functools import partial
import socket
import threading
import json
import hou

# 送信先のIPアドレスとポート番号
serv_address = ('127.0.0.1', 50001)
# UDPソケット
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# フラグで送信状態を管理
is_running = False
# スレッド同期のためのロック
lock = threading.Lock()

# テキストメッセージ送信関数
def send_message(message):
    message = message.text()
    if message:
        try:
            sock.sendto(message.encode('utf-8'), serv_address)  # メッセージを送信
            print(f"送信しました: {message}")
        except Exception as e:
            print(f"送信エラー: {e}")
    else:
        print("メッセージが空です。")

# パラメータ値を取得する関数
def get_parm_tuple(node, parm_name):
    try:
        return node.evalParmTuple(parm_name)
    except hou.OperationFailed:
        return None

# 頂点と法線情報を取得する関数
def get_vertex_and_normal_info(node):
    try:
        geo = node.geometry()
        vertices = []
        normals = []

        # 頂点情報を取得
        for point in geo.points():
            position = point.position()
            vertices.append((position.x(), position.y(), position.z()))

        # 法線情報を取得
        if geo.findVertexAttrib("N"):
            # 頂点属性として法線を取得
            for vertex in geo.iterVertices():
                normal = vertex.attribValue("N")
                normals.append((normal[0], normal[1], normal[2]))
        elif geo.findPointAttrib("N"):
            # ポイント属性として法線を取得
            for point in geo.points():
                normal = point.attribValue("N")
                normals.append((normal[0], normal[1], normal[2]))
        elif geo.findPrimAttrib("N"):
            # プリミティブ属性として法線を取得
            for prim in geo.prims():
                normal = prim.attribValue("N")
                normals.append((normal[0], normal[1], normal[2]))
        else:
            # 法線がない場合、デフォルト値
            normals = [(0.0, 0.0, 0.0)] * len(vertices)

        return vertices, normals
    except AttributeError:
        return [], []  # Geometryがない場合は空のリスト

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
    return {
        "Translate": absolute_translate,
        "Rotate": absolute_rotate,
        "Scale": absolute_scale
    }

# 頂点データと法線データを送信する関数
def send_vertex_and_normal_data():
    all_nodes = hou.node("/obj").allSubChildren()
    for node in all_nodes:
        # 頂点情報と法線情報を取得
        vertices, normals = get_vertex_and_normal_info(node)

        # 頂点がないノードはスキップ
        if not vertices:
            continue

        # ノード名（親の名前を含める）
        node_name = node.name()
        parent_name = node.parent().name() if node.parent() else None
        full_name = f"{parent_name}_{node_name}" if parent_name else node_name

        # 親ノードのTransform情報を取得
        parent_node = node.parent()
        parent_transform = {
            "Translate": (0, 0, 0),
            "Rotate": (0, 0, 0),
            "Scale": (1, 1, 1)
        }
        if parent_node is not None:
            parent_transform = calculate_absolute_transform(parent_node, {
                "Translate": (0, 0, 0),
                "Rotate": (0, 0, 0),
                "Scale": (1, 1, 1)
            })

        # Transform情報を計算
        transform_info = calculate_absolute_transform(node, parent_transform)

        # .obj形式のデータを整形
        obj_data = f"Name: {full_name}\n"
        obj_data += (
            f"Translate {transform_info['Translate'][0]:.3f} "
            f"{transform_info['Translate'][1]:.3f} "
            f"{transform_info['Translate'][2]:.3f}\n"
            f"Rotate {transform_info['Rotate'][0]:.3f} "
            f"{transform_info['Rotate'][1]:.3f} "
            f"{transform_info['Rotate'][2]:.3f}\n"
            f"Scale {transform_info['Scale'][0]:.3f} "
            f"{transform_info['Scale'][1]:.3f} "
            f"{transform_info['Scale'][2]:.3f}\n"
        )
        obj_data += "\n".join(f"v {v[0]:.3f} {v[1]:.3f} {v[2]:.3f}" for v in vertices) + "\n"
        obj_data += "\n".join(f"vn {n[0]:.3f} {n[1]:.3f} {n[2]:.3f}" for n in normals)

        # データを送信
        try:
            sock.sendto(obj_data.encode('utf-8'), serv_address)
            print(f"送信しました:\n{obj_data}")
        except Exception as e:
            print(f"送信エラー: {e}")

# 通信を開始
def start_sending():
    global is_running
    if is_running:
        print("既に通信が開始されています。")
        return
    with lock:
        is_running = True
    print("通信スレッドを開始しました。")

# 通信を停止
def stop_sending():
    global is_running
    if not is_running:
        print("通信は既に停止しています。")
        return
    with lock:
        is_running = False
    print("通信停止を要求しました。")

# ダイアログの作成
dialog = QtWidgets.QWidget()
dialog.setWindowTitle("レベルエディタ(改良版)")
dialog.setGeometry(100, 100, 500, 200)
dialog.setParent(hou.qt.mainWindow(), QtCore.Qt.Window)

# レイアウト
v_layout = QtWidgets.QVBoxLayout()

# テキストエディタ
text_editor = QtWidgets.QLineEdit()

# ボタン作成
send_button = QtWidgets.QPushButton("テキストを送信")
send_button.clicked.connect(partial(send_message, text_editor))

sendNode_button = QtWidgets.QPushButton("シーン情報を送信")
sendNode_button.clicked.connect(send_vertex_and_normal_data)

start_button = QtWidgets.QPushButton("通信開始")
start_button.clicked.connect(start_sending)

stop_button = QtWidgets.QPushButton("通信停止")
stop_button.clicked.connect(stop_sending)

# レイアウトにウィジェットを追加
v_layout.addWidget(text_editor)
v_layout.addWidget(send_button)
v_layout.addWidget(sendNode_button)
v_layout.addWidget(start_button)
v_layout.addWidget(stop_button)

dialog.setLayout(v_layout)
dialog.show()
