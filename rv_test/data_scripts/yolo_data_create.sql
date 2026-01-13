PRAGMA yt.InferSchema = '1000';
PRAGMA yt.IgnoreWeakSchema;
PRAGMA AnsiInForEmptyOrNullableItemsCollections;

$script = @@#py

def convert_to_yolo(category_id: int, image_height: float, image_width: float, bbox: list) -> str:
    if category_id not in [5, 16, 38]:
        return ""

    class_id = 0 if category_id == 5 else 1 if category_id == 16 else 2
    x_center = float(bbox[0] + bbox[2]/2.0) / image_width
    y_center = float(bbox[1] + bbox[3]/2.0)/ image_height
    width = float(bbox[2]) / image_width
    height = float(bbox[3]) / image_height

    return f"{class_id} {x_center:.6f} {y_center:.6f} {width:.6f} {height:.6f}"
@@;

$convert_to_yolo = Python3::convert_to_yolo(Callable <(Optional<Int64>, Optional<Float>, Optional<Float>, List<Optional<Double>>)->String?>, $script);

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

INSERT INTO `images_yolo_paths` WITH TRUNCATE
SELECT
    file_name AS image_file_name
FROM $file_names;

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

INSERT INTO `annotation_yolo_paths` WITH TRUNCATE
SELECT
    yolo_txt AS txt_file_name,
    yolo_annotation AS yolo_annotation
FROM $annotations_only;
