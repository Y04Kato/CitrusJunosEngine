// NOTE: このテンプレはprotoヘッダファイルを使用する前提のもの
// TODO: インクルードするファイル名を書き換える
#include "SOP_LevelEditor.hpp"
#include "SOP_LevelEditor.proto.h" // protoヘッダを使うことが前提

// NOTE: 最低限必要なヘッダファイルのインクルード
#include <UT/UT_StringHolder.h>
#include <UT/UT_DSOVersion.h>
#include <OP/OP_Operator.h>
#include <OP/OP_OperatorTable.h>
#include <PRM/PRM_TemplateBuilder.h>


// TODO: 任意のネームスペースを指定
using namespace SOP_LEVELEDITOR;

// TODO: カスタムSOPクラスを指定
// TODO: 任意の内部名を指定
const UT_StringHolder SOP_LevelEditor::theSOPTypeName("LevelEditor");

// NOTE: プラグインロード時にOP_OperatorTableにカスタムSOPを登録する
void newSopOperator(OP_OperatorTable* table) {
	table->addOperator(new OP_Operator(
		// TODO: 必要に応じて書き換える
		SOP_LevelEditor::theSOPTypeName,   // SOPの内部名
		"LevelEditor",                     // SOPのラベル名
		SOP_LevelEditor::myConstructor,    // 自身のインスタンスを返すメソッド
		SOP_LevelEditor::buildTemplates(), // パラメータテンプレートを指定
		0,                              // 最低限必要な入力数
		0,                              // 入力プラグの数
		nullptr,                        // Custom local variables (none)
		OP_FLAG_GENERATOR));            // Flag it as generator
}

// NOTE: SOPに持たせるパラメータを定義する
// TODO: .dsファイル、または.dsファイルと同様のルールで書かれたraw文字列を代入する
static const char* theDsFile = R"THEDSFILE({
    name        parameters
    parm {
        name    "TestParameter"      // 内部パラメータ名
        label   "TestParameter" // インターフェース表示名
        type    integer
        default { "0" }     // パラメータのデフォルト値
        range   { 2! 50 }   // インターフェース上での値の最低値と最大値
                            // 2!で2以下に絶対にならないようになる
                            // スライダーの最大値が50だが、手動で更に高く設定可能
        export  all         // パラメータをツールボックスに表示
                            // ノードの状態がビューポートに表示される
    }
    parm {
        name    "TestVecotr2"
        label   "TestVector2"
        type    vector2     // Vector2で管理
        size    2           // 要素2つをVector2に入れる
        default { "1" "0.3" }
    }
    parm {
        name    "TestVector3"
        label   "TestVector3"
        type    vector     // Vector3で管理
        size    3           // 要素3つをVector3に入れる
        default { "0" "0" "0" }
    }
    parm {
        name    "TestCheckBox"
        label   "TestCheckBox"
        type    toggle  // チェックボックス
        default { "0" } // 0なのでFalseを初期値に
    }
    parm {
        name    "TestDialog"
        label   "TestDialog"
        type    ordinal     // ダイアログで管理
        default { "0" }     // 0番目の項目を初期値にする
        menu    {
            "Test1"    "Test1"
            "Test2"    "Test2"
            "Test3"    "Test3"
        }     // 内部パラメータ名とインターフェース表示名
    }
}
)THEDSFILE";

// NOTE: *theDsFile で記述された内容をもとにパラメータテンプレートを生成する
// TODO: カスタムSOPクラスを指定
// TODO: 正しいcppファイル名に書き換える
PRM_Template* SOP_LevelEditor::buildTemplates() {
	static PRM_TemplateBuilder templ("SOP_LevelEditor.cpp", theDsFile);
	return templ.templates();
}

// NOTE: Verbの仕組みを使うため、カスタムNodeVerbクラスを宣言
// TODO: カスタムNodeVerbクラス名を指定
class SOP_LevelEditorVerb : public SOP_NodeVerb {
public:
	// NOTE: デフォルトコンストラクタとデストラクタ
	// TODO: カスタムNodeVerbクラス名を指定
	SOP_LevelEditorVerb() {}
	virtual ~SOP_LevelEditorVerb() {}

	// TODO: protoヘッダで定義されているパラメータテンプレートクラスを指定
	virtual SOP_NodeParms* allocParms() const {
		return new SOP_LevelEditorParms();
	}

	// NOTE: theVerbメンバ変数の宣言
	// TODO: カスタムNodeVerbクラスを指定
	static const SOP_NodeVerb::Register<SOP_LevelEditorVerb> theVerb;

	virtual UT_StringHolder name() const {
		// TODO: カスタムSOPクラスを指定
		return SOP_LevelEditor::theSOPTypeName;
	}

	// NOTE: 編集不要
	virtual CookMode cookMode(const SOP_NodeParms* parms) const {
		return COOK_GENERIC;
	}

	// NOTE: 編集不要
	virtual void cook(const CookParms& cookparms) const;
};

// NOTE: 静的なメンバ変数の定義はクラス定義の外に書く
// TODO: カスタムNodeVerbクラスを指定
const SOP_NodeVerb::Register<SOP_LevelEditorVerb> SOP_LevelEditorVerb::theVerb;

// TODO: カスタムSOPクラスを指定
const SOP_NodeVerb* SOP_LevelEditor::cookVerb() const {
	return SOP_LevelEditorVerb::theVerb.get();
}

// NOTE: SOPのメイン処理 Verbを使う場合はSOP_NodeVerbのcookに実際の処理を委譲する
// TODO: カスタムNodeVerbクラスを指定
void SOP_LevelEditorVerb::cook(const SOP_NodeVerb::CookParms& cookparms) const {
	// TODO: ノードのメイン処理はここに記述する
}