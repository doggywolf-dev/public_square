import os
import torch
from ultralytics import YOLO
import time

# Загрузка предобученной модели
model1 = YOLO('best_stage_2_0.pt')
start_time = time.time()

# Запуск обучения модели
model1.train(
    data='dataset.yaml',      # файл конфигурации датасета в текущей папке
    epochs=50,
    imgsz=640,
    batch=24,
    workers=16,
    cache='ram',
    device='cpu',
    plots=True,

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
    mixup=0.1,          # вероятность MixUp-аугментации
    copy_paste=0.2,     # вероятность Copy-Paste аугментации

    patience=15,        # ранняя остановка при отсутствии улучшения
    lr0=0.0015,         # начальная скорость обучения

    name='stage2_light_aug_640px' # имя эксперимента для логирования
)

end_time = time.time()
training_time = end_time - start_time

print(f"Обучение завершено за: {training_time/60:.1f} минут")
