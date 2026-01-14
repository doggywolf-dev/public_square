from ultralytics import YOLO
import time

# Загрузка предобученной модели
model = YOLO('best_stage_0.pt')

start_time = time.time()

# Запуск обучения модели
model1.train(
    data='dataset.yaml',# файл конфигурации датасета в текущей папке
    epochs=20,
    imgsz=320,
    batch=12,           # оптимизация использования ресурсов
    workers=8,          # оптимизация использования ресурсов
    cache='ram',
    device='cpu',

    # Мягкие аугментации
    hsv_h=0.015,        # небольшие изменения оттенка
    hsv_s=0.7,          # изменения насыщенности
    hsv_v=0.4,          # изменения яркости
    degrees=10,         # небольшие повороты (в градусах)
    translate=0.1,      # небольшие сдвиги (в долях изображения)
    scale=0.2,          # изменения масштаба (до 20%)
    shear=2,            # небольшой сдвиг (в градусах)
    fliplr=0.5,         # вероятность горизонтального отражения
    mosaic=1.0,         # использование мозаичной аугментации
    mixup=0.0,          # пока без mixup
    copy_paste=0.0,     # пока без copy-paste

    lr0=0.002,          # начальная скорость обучения
    warmup_epochs=2,

    name='stage1_light_aug_320px' # имя эксперимента для логирования
)

end_time = time.time()
training_time = end_time - start_time

print(f"Обучение завершено за: {training_time/60:.1f} минут")
