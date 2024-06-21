// NOTE: このテンプレはprotoヘッダファイルを使用する前提のもの
// TODO: インクルードするファイル名を書き換える
#include "SOP_Template.hpp"
#include "SOP_Template.proto.h" // protoヘッダを使うことが前提

// NOTE: 最低限必要なヘッダファイルのインクルード
#include <UT/UT_StringHolder.h>
#include <UT/UT_DSOVersion.h>
#include <OP/OP_Operator.h>
#include <OP/OP_OperatorTable.h>
#include <PRM/PRM_TemplateBuilder.h>


// TODO: 任意のネームスペースを指定
using namespace SOP_TEMPLATE;

// TODO: カスタムSOPクラスを指定
// TODO: 任意の内部名を指定
const UT_StringHolder SOP_Template::theSOPTypeName("template");

// NOTE: プラグインロード時にOP_OperatorTableにカスタムSOPを登録する
void newSopOperator(OP_OperatorTable* table){
    table->addOperator(new OP_Operator(
        // TODO: 必要に応じて書き換える
        SOP_Template::theSOPTypeName,   // SOPの内部名
        "template",                     // SOPのラベル名
        SOP_Template::myConstructor,    // 自身のインスタンスを返すメソッド
        SOP_Template::buildTemplates(), // パラメータテンプレートを指定
        0,                              // 最低限必要な入力数
        0,                              // 入力プラグの数
        nullptr,                        // Custom local variables (none)
        OP_FLAG_GENERATOR));            // Flag it as generator
}

// NOTE: SOPに持たせるパラメータを定義する
// TODO: .dsファイル、または.dsファイルと同様のルールで書かれたraw文字列を代入する
static const char* theDsFile = R"THEDSFILE({
        name        parameters
}
)THEDSFILE";

// NOTE: *theDsFile で記述された内容をもとにパラメータテンプレートを生成する
// TODO: カスタムSOPクラスを指定
// TODO: 正しいcppファイル名に書き換える
PRM_Template* SOP_Template::buildTemplates(){
    static PRM_TemplateBuilder templ("SOP_Template.cpp", theDsFile);
    return templ.templates();
}

// NOTE: Verbの仕組みを使うため、カスタムNodeVerbクラスを宣言
// TODO: カスタムNodeVerbクラス名を指定
class SOP_TemplateVerb : public SOP_NodeVerb{
public:
    // NOTE: デフォルトコンストラクタとデストラクタ
    // TODO: カスタムNodeVerbクラス名を指定
    SOP_TemplateVerb() {}
    virtual ~SOP_TemplateVerb() {}

    // TODO: protoヘッダで定義されているパラメータテンプレートクラスを指定
    virtual SOP_NodeParms* allocParms() const{
        return new SOP_TemplateParms();
    }

    // NOTE: theVerbメンバ変数の宣言
    // TODO: カスタムNodeVerbクラスを指定
    static const SOP_NodeVerb::Register<SOP_TemplateVerb> theVerb;

    virtual UT_StringHolder name() const{
        // TODO: カスタムSOPクラスを指定
        return SOP_Template::theSOPTypeName;
    }

    // NOTE: 編集不要
    virtual CookMode cookMode(const SOP_NodeParms* parms) const{
        return COOK_GENERIC;
    }

    // NOTE: 編集不要
    virtual void cook(const CookParms& cookparms) const;
};

// NOTE: 静的なメンバ変数の定義はクラス定義の外に書く
// TODO: カスタムNodeVerbクラスを指定
const SOP_NodeVerb::Register<SOP_TemplateVerb> SOP_TemplateVerb::theVerb;

// TODO: カスタムSOPクラスを指定
const SOP_NodeVerb* SOP_Template::cookVerb() const{
    return SOP_TemplateVerb::theVerb.get();
}

// NOTE: SOPのメイン処理 Verbを使う場合はSOP_NodeVerbのcookに実際の処理を委譲する
// TODO: カスタムNodeVerbクラスを指定
void SOP_TemplateVerb::cook(const SOP_NodeVerb::CookParms& cookparms) const{
    // TODO: ノードのメイン処理はここに記述する
}