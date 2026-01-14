from ultralytics import YOLO
import time

# Загрузка предобученной модели
model = YOLO('yolov8n.pt')

start_time = time.time()

# Запуск обучения модели
model.train(
    data='dataset.yaml',    # файл конфигурации датасета в текущей папке
    epochs=10,
    imgsz=320,
    batch=4,
    device='cpu',
    patience=5,
    cache=False,

    plots=False,
    verbose=True,

    name='timed_training', # имя эксперимента для логирования
)

end_time = time.time()
training_time = end_time - start_time

print(f"Обучение завершено за: {training_time/60:.1f} минут")
