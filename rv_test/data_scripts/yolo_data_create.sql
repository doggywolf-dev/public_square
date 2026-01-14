PRAGMA yt.InferSchema = '1000';
PRAGMA yt.IgnoreWeakSchema;
PRAGMA AnsiInForEmptyOrNullableItemsCollections;

-- Python-скрипт для конвертации аннотаций в формат YOLO
$script = @@#py

def convert_to_yolo(category_id: int, image_height: float, image_width: float, bbox: list) -> str:
    # Обработка только целевых категорий: 5 (самолет), 16 (птица), 38 (воздушный змей)
    if category_id not in [5, 16, 38]:
        return ""

    # Преобразование ID категории в ID класса для YOLO (0, 1, 2)
    class_id = 0 if category_id == 5 else 1 if category_id == 16 else 2

    # Вычисление нормализованных координат центра объекта
    x_center = float(bbox[0] + bbox[2]/2.0) / image_width
    y_center = float(bbox[1] + bbox[3]/2.0)/ image_height

    # Вычисление нормализованных размеров объекта
    width = float(bbox[2]) / image_width
    height = float(bbox[3]) / image_height

    # Возврат строки в формате YOLO
    return f"{class_id} {x_center:.6f} {y_center:.6f} {width:.6f} {height:.6f}"
@@;

-- Регистрация Python-функции для использования в SQL
$convert_to_yolo = Python3::convert_to_yolo(Callable <(Optional<Int64>, Optional<Float>, Optional<Float>, List<Optional<Double>>)->String?>, $script);

-- Объединение всех изображений из наборов set1 и set2
$united = (
    SELECT
        "1" AS `set`,
        String::SplitToList(TableName(), "/")[0] AS `type`,
        image_id AS image_id
    FROM
        Range(`set1/`, ``, ``, ``)
    UNION ALL
    SELECT
        "2" AS `set`,
        String::SplitToList(TableName(), "/")[0] AS `type`,
        image_id AS image_id
    FROM
        Range(`set2/`, ``, ``, ``)
);

-- Получение уникальных изображений с их наборами и типами
$unique_images = (
    SELECT
        image_id,
        `set`,
        `type`
    FROM
        $united
    GROUP BY
        image_id, `set`, `type`
);

-- Формирование путей к файлам изображений и меток YOLO
$file_names = (
    SELECT
        a.image_id AS image_id,
        b.file_name AS image_name,
        'dataset' || `set` || '/images/' || `type` || '/' || b.file_name AS file_name,
        b.height AS height,
        b.width AS width,
        'dataset' || `set` || '/labels/' || `type` || '/' || String::SplitToList(b.file_name, ".")[0] || ".txt" AS yolo_txt
    FROM
        $unique_images AS a
    INNER JOIN CONCAT(`instances_val_2017_images`, `instances_train_2017_images`) AS b ON a.image_id == b.id
);

-- Сохранение путей к изображениям
INSERT INTO `images_yolo_paths` WITH TRUNCATE
SELECT
    file_name AS image_file_name
FROM $file_names;

-- Добавление аннотаций к изображениям и конвертация в формат YOLO
$with_annotations = (
    SELECT
        a.*,
        IF(b.image_id IS NOT null,
        $convert_to_yolo(
            b.category_id,
            CAST(a.height AS Float?),
            CAST(a.width AS Float?),
            AsList(
                Yson::ConvertToDouble(Yson::ConvertToList(b.bbox)[0]),
                Yson::ConvertToDouble(Yson::ConvertToList(b.bbox)[1]),
                Yson::ConvertToDouble(Yson::ConvertToList(b.bbox)[2]),
                Yson::ConvertToDouble(Yson::ConvertToList(b.bbox)[3])
            )
        ),
        ""
        ) AS yolo_annotation,
        b.bbox AS annotations,
        b.category_id AS category_id
    FROM
        $file_names AS a
    LEFT JOIN CONCAT(`instances_val_2017_annotations`, `instances_train_2017_annotations`) AS b ON a.image_id == b.image_id
);

-- Группировка всех аннотаций для одного изображения
$annotations_only = (
    SELECT
        yolo_txt,
        ListConcat(AGGREGATE_LIST(yolo_annotation), '\n') AS yolo_annotation
    FROM
        $with_annotations
    WHERE
        yolo_annotation != ""
    GROUP BY yolo_txt
);

-- Сохранение файлов с метками YOLO
INSERT INTO `annotation_yolo_paths` WITH TRUNCATE
SELECT
    yolo_txt AS txt_file_name,
    yolo_annotation AS yolo_annotation
FROM $annotations_only;
