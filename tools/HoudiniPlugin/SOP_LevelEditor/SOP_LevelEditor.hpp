// TODO: インクルードガードはカスタムSOPクラス名に合わせ指定
#ifndef __SOP_LevelEditor_h__
#define __SOP_LevelEditor_h__

// NOTE: 編集不要
#include <SOP/SOP_Node.h>
#include <UT/UT_StringHolder.h>

// TODO: ネームスペースを書き換える
namespace SOP_LEVELEDITOR{
    // NOTE: ここではSOP_Nodeクラスを継承し、カスタムのSOPクラスを宣言する
    // TODO: カスタムSOPクラスを指定
    class SOP_LevelEditor : public SOP_Node{
    public:
        static OP_Node* myConstructor(OP_Network* net, const char* name, OP_Operator* op){
            // NOTE: 自身のインスタンスを返すようにする
            // TODO: カスタムSOPクラスを指定
            return new SOP_LevelEditor(net, name, op);
        }

        // NOTE: 以下は編集不要
        static PRM_Template* buildTemplates();                 // パラメータテンプレートクラスを返す
        virtual const SOP_NodeVerb* cookVerb() const override; // Verbの仕組みで処理を行うメソッド
        static const UT_StringHolder theSOPTypeName;           // ノードタイプの内部名を格納する変数

    protected:
        // NOTE: コンストラクタを定義
        // TODO: カスタムSOPクラス名を指定
        SOP_LevelEditor(OP_Network* net, const char* name, OP_Operator* op) : SOP_Node(net, name, op){
            mySopFlags.setManagesDataIDs(true);
        }

        // NOTE: デストラクタを定義
        // TODO: カスタムSOPクラス名を指定
        virtual ~SOP_LevelEditor() {}

        // NOTE: 以下は編集不要
        virtual OP_ERROR cookMySop(OP_Context& context) override{
            return cookMyselfAsVerb(context);
        }
    };
} // namespace SOP_LevelEditor

#endif