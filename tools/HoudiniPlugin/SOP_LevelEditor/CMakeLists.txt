# NOTE: 変更不要 対応するcmakeの最小バージョン
cmake_minimum_required( VERSION 3.6 )

# NOTE: sln内に作成するプロジェクト名
# TODO: 任意の名前を指定。カスタムSOPクラス名が良さそう
project(SOP_LevelEditor)

# NOTE: 変更不要 cmakeファイル検索パスを追加
list( APPEND CMAKE_PREFIX_PATH "$ENV{HFS}/toolkit/cmake" )

# NOTE: Houdiniが用意しているcmake関数などを含むパッケージを読みこむ
find_package( Houdini REQUIRED )

# NOTE: ライブラリ名変数にカスタムSOPクラス名の文字列を代入しておく
# TODO: ライブラリ名を変更SOP_TemplateのようにカスタムSOPクラス名にするとよい
set(library_name SOP_LevelEditor)

# NOTE: ここの指定でprotoヘッダが作られるようになる
# TODO: ソースコードファイル名(.c/.cpp)を記述
houdini_generate_proto_headers( FILES SOP_LevelEditor.cpp )

# NOTE: ライブラリに追加するファイルを記述
# TODO: 必要なcpp/hppファイルを羅列する
add_library( ${library_name} SHARED
    SOP_LevelEditor.cpp
    SOP_LevelEditor.hpp
)

# NOTE: 変更不要
target_link_libraries( ${library_name} Houdini )

# NOTE: 変更不要
target_include_directories( ${library_name} PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
)

# NOTE: 変更不要
# TODO: 必要に応じてオプションを追加
houdini_configure_target( ${library_name} )