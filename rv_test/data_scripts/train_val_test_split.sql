PRAGMA yt.InferSchema = '1000';
PRAGMA yt.IgnoreWeakSchema;
PRAGMA AnsiInForEmptyOrNullableItemsCollections;

-- Объединение категорий и изображений из обучающей и валидационной выборок в уникальные пары
$cats_n_images_only = (
    SELECT
        category_id AS category_id,
        image_id AS image_id,
    FROM (
        SELECT
            category_id AS category_id,
            image_id AS image_id
        FROM
            `instances_val_2017_annotations`
        UNION ALL
        SELECT
            category_id AS category_id,
            image_id AS image_id
        FROM
            `instances_train_2017_annotations`
    )
    GROUP BY category_id, image_id
);

-- Позитивная выборка: изображения с категориями 5 (самолет), 16 (птица), 38 (воздушный змей)
$positive_set = (
    SELECT
        image_id AS image_id,
        AGGREGATE_LIST_DISTINCT(category_id) AS categories,
    FROM
        $cats_n_images_only
    WHERE
        category_id IN (5, 16, 38)
    GROUP BY image_id
);

-- Негативная выборка: изображения без целевых категорий
$negative_set = (
    SELECT
        image_id AS image_id,
        AGGREGATE_LIST_DISTINCT(category_id) AS categories,
        random(image_id) AS rand
    FROM
        $cats_n_images_only
    WHERE
        image_id NOT IN (SELECT image_id FROM $positive_set)
    GROUP BY image_id
);

-- Пустая выборка: изображения без аннотаций
$blank_set = (
    SELECT
        id AS image_id,
        AsList() AS categories,
        rand AS rand
    FROM (
        SELECT
            *
        FROM
            `instances_val_2017_images`
        UNION ALL
        SELECT
            *
        FROM
            `instances_train_2017_images`
    )
    WHERE id NOT IN (SELECT image_id FROM $cats_n_images_only)
    GROUP BY id, random(id) AS rand
);

-- Рабочий набор: все позитивные + 784 негативных + 184 пустых изображения
$work_set = (
    SELECT
        a.*,
        random(image_id) AS rand
    FROM (
        SELECT
            *
        FROM
            $positive_set
        UNION ALL
        SELECT
            *
        FROM (
            SELECT
                image_id AS image_id,
                categories AS categories
            FROM
                $negative_set
            ORDER BY rand
            LIMIT 784
        )
        UNION ALL
        SELECT
            *
        FROM (
            SELECT
                image_id AS image_id,
                categories AS categories
            FROM
                $blank_set
            ORDER BY rand
            LIMIT 184
        )
    ) AS a
);

-- Золотой стандарт: 150 случайных изображений для оценки качества
$golden_set = (
    SELECT
        *
    FROM
        $work_set
    ORDER BY rand
    LIMIT 150
);

-- Сохранение золотого стандарта в первый набор
INSERT INTO `set1/golden_set` WITH TRUNCATE
SELECT
    image_id AS image_id,
    categories AS categories
FROM
    $golden_set;

-- Подсчет статистики по категориям в золотом стандарте
SELECT
    COUNT_IF(5 IN categories) AS airplane_count,
    COUNT_IF(16 IN categories) AS bird_count,
    COUNT_IF(38 IN categories) AS kite_count,
    COUNT_IF(ListLength(categories) == 0) AS blank_count,
    COUNT_IF((ListLength(categories) > 0) and not (5 IN categories) and not (16 IN categories) and not (38 IN categories)) AS other_count
FROM
    $golden_set;

-- Тестовая выборка 1: 1906 изображений (исключая золотой стандарт)
$test_set1 = (
    SELECT
        image_id AS image_id,
        categories AS categories
    FROM
        $work_set
    WHERE image_id not IN (select image_id from $golden_set)
    ORDER BY rand
    LIMIT 1906
);

-- Сохранение тестовой выборки 1
INSERT INTO `set1/test` WITH TRUNCATE
SELECT
    *
FROM
    $test_set1;

-- Статистика тестовой выборки 1
SELECT
    COUNT_IF(5 IN categories) AS airplane_count,
    COUNT_IF(16 IN categories) AS bird_count,
    COUNT_IF(38 IN categories) AS kite_count,
    COUNT_IF(ListLength(categories) == 0) AS blank_count,
    COUNT_IF((ListLength(categories) > 0) and not (5 IN categories) and not (16 IN categories) and not (38 IN categories)) AS other_count
FROM
    $test_set1;

-- Валидационная выборка 1: 1906 изображений (исключая золотой стандарт и тест 1)
$validate_set1 = (
    SELECT
        image_id AS image_id,
        categories AS categories
    FROM
        $work_set
    WHERE (image_id not IN (select image_id from $golden_set)) and (image_id not IN (select image_id from $test_set1))
    ORDER BY rand
    LIMIT 1906
);

-- Сохранение валидационной выборки 1
INSERT INTO `set1/val` WITH TRUNCATE
SELECT
    *
FROM
    $validate_set1;

-- Статистика валидационной выборки 1
SELECT
    COUNT_IF(5 IN categories) AS airplane_count,
    COUNT_IF(16 IN categories) AS bird_count,
    COUNT_IF(38 IN categories) AS kite_count,
    COUNT_IF(ListLength(categories) == 0) AS blank_count,
    COUNT_IF((ListLength(categories) > 0) and not (5 IN categories) and not (16 IN categories) and not (38 IN categories)) AS other_count
FROM
    $validate_set1;

-- Обучающая выборка 1: все оставшиеся изображения (исключая золотой стандарт, тест 1 и валидацию 1)
$train_set1 = (
    SELECT
        image_id AS image_id,
        categories AS categories
    FROM
        $work_set
    WHERE
        (image_id not IN (select image_id from $golden_set))
        and (image_id not IN (select image_id from $test_set1))
        and (image_id not IN (select image_id from $validate_set1))
);

-- Сохранение обучающей выборки 1
INSERT INTO `set1/train` WITH TRUNCATE
SELECT
    *
FROM
    $train_set1;

-- Статистика обучающей выборки 1
SELECT
    COUNT_IF(5 IN categories) AS airplane_count,
    COUNT_IF(16 IN categories) AS bird_count,
    COUNT_IF(38 IN categories) AS kite_count,
    COUNT_IF(ListLength(categories) == 0) AS blank_count,
    COUNT_IF((ListLength(categories) > 0) and not (5 IN categories) and not (16 IN categories) and not (38 IN categories)) AS other_count
FROM
    $train_set1;
