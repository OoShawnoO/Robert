# Robert
## Develop Environment / 开发环境
- Linux (WSL Ubuntu 22.04)
- C++ 11 ↑
- onnxruntime-linux-[cpu/gpu] 1.15.1 ↑ _[Require]_
- onnxruntime-linux-gpu 1.15.1 ↑ _[Optional]_
- opencv 4.7.0 _[Require]_
- Qt 5.15.2 _[Require]_

## Third Party
> **json for modern C++ version 3.11.2 ↑**
> - author: *nlohmann*
> - path: _Utils/include_
> - url:_https://github.com/nlohmann/json.git_

> **ONNX Runtime is a cross-platform inference and training machine-learning accelerator. v1.15.1 ↑**
> - author: *microsoft*
> - path: _Service/include/Yolo_
> - url:_https://github.com/microsoft/onnxruntime.git

> **OpenCV: Open Source Computer Vision Library 4.7.0 ↑**
> - author: *opencv*
> - path: _Utils/include_
> - url:_https://github.com/opencv/opencv.git_

> **nodeedtior: Qt Node Editor. Dataflow programming framework 3.0.10 ↑**
> - author: *paceholder*
> - path: _Terminal/thirdparty/\*/QtNodes_
> - url:_https://github.com/paceholder/nodeeditor.git_


## Usage / 用法
```shell
git clone https://github.com/OoShawnoO/Robert.git
mkdir build && cd build
cmake ..
make
```

## Introduction / 简介
When deploying different solutions for different YOLO models, it is usually necessary to repeatedly write similar logic, such as target contact, target distance interval, or some more complex processes or logic.

当需要为不同的yolo模型进行不同方案的部署时，通常需要反复编写相似的逻辑，如目标接触、目标距离区间，或一些更复杂的流程或逻辑。

Can simple logic be concatenated and combined into complex logic to improve efficiency to complete a new solution?

为了提高效率是否可以将简单逻辑串联组合为复杂逻辑以完成新的解决方案?

Robert will be developed for simultaneous inference and interaction among multiple models, with a graphical editor format for customized editing process solutions.

Robert 将被开发用于多模型同时推理与交互，图形化编辑器形式客制化编辑流程方案。

## Module / 模块
- **Service**
    
    Responsible for camera management, video stream processing, inference services, and process error correction.
    
    负责摄像头管理、视频流处理、推理服务及流程纠错等。
- **Terminal**

  Responsible for graphical user interface, customized process editing, and image display after processing.    

  负责图形化用户界面、客制化流程编辑、处理后图像显示等。

- **Utils**

  Utility tools,such as log、thread-safe queue、configure control and so on.
  
  通用工具,日志、线程安全队列、配置文件管理等。

## Interactive process / 交互流程
![](doc/单机交互流程.svg)