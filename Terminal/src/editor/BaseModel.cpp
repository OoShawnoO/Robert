/**
  ******************************************************************************
  * @file           : BaseModel.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/12/17
  ******************************************************************************
  */

#include "BaseModel.h"

namespace hzd {
    ModelBroadcaster BaseModel::broadcaster;

    BaseModel::BaseModel(std::string name, std::string caption)
    :_name(std::move(name)),_caption(std::move(caption)){
        widget.setStyleSheet("background-color:transparent");
        layout.setContentsMargins(0, 0, 0, 0);
        layout.setSpacing(0);
    }

    BaseModel::~BaseModel() = default;

    QString BaseModel::name() const {
        return {_name.c_str()};
    }

    QJsonObject BaseModel::save() const {
        return NodeDelegateModel::save();
    }

    QString BaseModel::caption() const {
        return {_caption.c_str()};
    }

    bool BaseModel::captionVisible() const {
        return NodeDelegateModel::captionVisible();
    }

    QWidget *BaseModel::embeddedWidget() {
        return &widget;
    }

    void BaseModel::load(const QJsonObject &p) {
        NodeDelegateModel::load(p);
    }

    unsigned int BaseModel::nPorts(PortType portType) const {
        return 1;
    }

    NodeDataType BaseModel::dataType(PortType portType, PortIndex portIndex) const {
        return {};
    }

    void BaseModel::setInData(std::shared_ptr<NodeData> nodeData, const PortIndex portIndex) {

    }

    std::shared_ptr<NodeData> BaseModel::outData(const PortIndex port) {
        return {};
    }

    void ModelBroadcaster::UpdateMap(const std::string &name, int id) {
        nameIdMap[name] = id;
        emit updated();
    }

    void ModelBroadcaster::EraseMap(const std::string &name) {
        nameIdMap.erase(name);
        emit updated();
    }

    bool ModelBroadcaster::HasKey(const std::string &name) {
        return nameIdMap.find(name) != nameIdMap.end();
    }

    std::vector<std::string> ModelBroadcaster::Keys() const {
        std::vector<std::string> keys;
        for(const auto& p : nameIdMap) {
            keys.emplace_back(p.first);
        }
        return std::move(keys);
    }

    void ModelBroadcaster::Serialize(QJsonObject &modelJson) {
        QJsonObject mapJson;
        for(const auto& p : nameIdMap) {
            mapJson[p.first.c_str()] = p.second;
        }
        modelJson["nameIdMap"] = mapJson;
        isSerialized = true;
    }

    bool ModelBroadcaster::Deserialize(const QJsonValue &p) {
        auto object = p.toObject();
        for(const auto& key : object.keys()) {
            nameIdMap[key.toStdString()] = object[key].toInt();
        }
        isSerialized = false;
    }

    int ModelBroadcaster::At(const std::string &key) {
        return nameIdMap.at(key);
    }
} // hzd