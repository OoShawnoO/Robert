/**
  ******************************************************************************
  * @file           : BaseModel.h
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/12/17
  ******************************************************************************
  */

#ifndef ROBERT_BASEMODEL_H
#define ROBERT_BASEMODEL_H

#include "QRowEdit.h"
#include "QRowButton.h"
#include <NodeDelegateModel>
#include <QVBoxLayout>
#include <QPushButton>


using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace hzd {

#define PARSE_MODEL_JSON(json,component, key) do{   \
    QJsonValue object = json[key];                  \
    if(!object.isUndefined()){                      \
        component.SetCurrentText(object.toString());\
    }                                               \
}while(0)

    class ModelBroadcaster : public QObject {
        Q_OBJECT
        std::unordered_map<std::string,int> nameIdMap;
    public:
        void UpdateMap(const std::string& name,int id);
        void EraseMap(const std::string& name);
        bool HasKey(const std::string& name);
        std::vector<std::string> Keys() const;
        void Serialize(QJsonObject& modelJson);
        bool Deserialize(const QJsonValue& p);
        int At(const std::string& key);
    signals:
        void updated();
    };

    class BaseModel : public NodeDelegateModel {
    Q_OBJECT
    public:
        BaseModel(std::string name,std::string caption);
        ~BaseModel() override;
        QString                     name() const override;
        QJsonObject                 save() const override;
        QString                     caption() const override;
        bool                        captionVisible() const override;
        QWidget*                    embeddedWidget()override;
        void                        load(QJsonObject const &p) override;
        unsigned int                nPorts(PortType portType) const override;
        NodeDataType dataType(PortType portType, PortIndex portIndex) const override;
        void setInData(std::shared_ptr<NodeData> nodeData, const PortIndex portIndex) override;
        std::shared_ptr<NodeData> outData(const PortIndex port) override;
        static ModelBroadcaster broadcaster;
    protected:
        QWidget widget;
        QVBoxLayout layout;
        std::string _name;
        std::string _caption;

    };

} // hzd

#endif //ROBERT_BASEMODEL_H
