from ultralytics import YOLO

model = YOLO('best.pt')  # загрузка модели

model.export(
    format='onnx',
    imgsz=640,           # размер, на котором обучалась модель, максимально близкий к HD
    device='cpu',        # можно и GPU, но не обязательно
    opset=12,            # стандартный opset
    simplify=True        # eghjotybt uhfaf
)