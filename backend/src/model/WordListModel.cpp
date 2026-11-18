#include "WordListModel.h"

WordListModel::WordListModel(QObject *parent)
    : QAbstractListModel(parent) {
    loadMockData();
}

int WordListModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return m_displayItems.size();
}

QVariant WordListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_displayItems.size()) {
        return QVariant();
    }

    const auto &item = m_displayItems.at(index.row());

    switch (role) {
    case IdRole:
        return item.id;
    case WordRole:
    case Qt::DisplayRole:
        return item.word;
    case PhoneticRole:
        return item.phonetic;
    case PartOfSpeechRole:
        return item.partOfSpeech;
    case MeaningViRole:
        return item.meaningVi;
    case DefinitionEnRole:
        return item.definitionEn;
    case ExampleSentenceRole:
        return item.exampleSentence;
    case ExampleTranslationRole:
        return item.exampleTranslation;
    case HintImageRole:
        return item.hintImage;
    case AudioPathRole:
        return item.audioPath;
    case DifficultyLevelRole:
        return item.difficultyLevel;
    case TopicIdRole:
        return item.topicId;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> WordListModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[WordRole] = "word";
    roles[PhoneticRole] = "phonetic";
    roles[PartOfSpeechRole] = "partOfSpeech";
    roles[MeaningViRole] = "meaningVi";
    roles[DefinitionEnRole] = "definitionEn";
    roles[ExampleSentenceRole] = "exampleSentence";
    roles[ExampleTranslationRole] = "exampleTranslation";
    roles[HintImageRole] = "hintImage";
    roles[AudioPathRole] = "audioPath";
    roles[DifficultyLevelRole] = "difficultyLevel";
    roles[TopicIdRole] = "topicId";
    return roles;
}

void WordListModel::loadMockData() {
    QVector<WordItem> mockList = {
        {1, QStringLiteral("Algorithm"), QStringLiteral("/ˈæl.ɡə.rɪ.ðəm/"), QStringLiteral("noun"),
         QStringLiteral("Thuật toán"), QStringLiteral("A set of mathematical instructions or rules for solving a problem."),
         QStringLiteral("The sorting algorithm executes in linear time."), QStringLiteral("Thuật toán sắp xếp thực thi trong thời gian tuyến tính."),
         QStringLiteral("assets/images/algorithm.png"), QStringLiteral("assets/audio/algorithm.mp3"), 1, 1},

        {2, QStringLiteral("Encapsulation"), QStringLiteral("/ɪnˌkæp.sjəˈleɪ.ʃən/"), QStringLiteral("noun"),
         QStringLiteral("Đóng gói dữ liệu"), QStringLiteral("The bundling of data and the methods that act on that data."),
         QStringLiteral("Encapsulation is one of the four fundamental OOP concepts."), QStringLiteral("Đóng gói là một trong bốn khái niệm cơ bản của OOP."),
         QStringLiteral("assets/images/encapsulation.png"), QStringLiteral("assets/audio/encapsulation.mp3"), 2, 1},

        {3, QStringLiteral("Polymorphism"), QStringLiteral("/ˌpɒl.iˈmɔː.fɪ.zəm/"), QStringLiteral("noun"),
         QStringLiteral("Tính đa hình"), QStringLiteral("The ability of different objects to respond in unique ways to the same message."),
         QStringLiteral("Polymorphism allows treating derived classes through a base pointer."), QStringLiteral("Tính đa hình cho phép xử lý các lớp dẫn xuất qua con trỏ cơ sở."),
         QStringLiteral("assets/images/polymorphism.png"), QStringLiteral("assets/audio/polymorphism.mp3"), 2, 1},

        {4, QStringLiteral("Inheritance"), QStringLiteral("/ɪnˈher.ɪ.təns/"), QStringLiteral("noun"),
         QStringLiteral("Tính kế thừa"), QStringLiteral("Mechanism where a new class acquires properties of an existing class."),
         QStringLiteral("Inheritance facilitates code reuse and hierarchical organization."), QStringLiteral("Kế thừa tạo điều kiện tái sử dụng mã nguồn và tổ chức phân cấp."),
         QStringLiteral("assets/images/inheritance.png"), QStringLiteral("assets/audio/inheritance.mp3"), 1, 1},

        {5, QStringLiteral("Repository"), QStringLiteral("/rɪˈpɒz.ɪ.tər.i/"), QStringLiteral("noun"),
         QStringLiteral("Kho lưu trữ / Mẫu Repository"), QStringLiteral("A design pattern that mediates between data source and business logic."),
         QStringLiteral("The repository abstracts database operations cleanly."), QStringLiteral("Repository trừu tượng hóa các thao tác cơ sở dữ liệu một cách rõ ràng."),
         QStringLiteral("assets/images/repository.png"), QStringLiteral("assets/audio/repository.mp3"), 3, 2},

        {6, QStringLiteral("Controller"), QStringLiteral("/kənˈtrəʊ.lər/"), QStringLiteral("noun"),
         QStringLiteral("Bộ điều khiển"), QStringLiteral("A component that coordinates user requests and business logic."),
         QStringLiteral("The bridge controller synchronizes React and C++."), QStringLiteral("Bộ điều khiển cầu nối đồng bộ React và C++."),
         QStringLiteral("assets/images/controller.png"), QStringLiteral("assets/audio/controller.mp3"), 1, 2}
    };

    m_allItems = mockList;
    updateDisplayItems(mockList);
}

QVariantMap WordListModel::get(int index) const {
    if (index < 0 || index >= m_displayItems.size()) {
        return QVariantMap();
    }
    const auto &item = m_displayItems.at(index);
    QVariantMap map;
    map[QStringLiteral("id")] = item.id;
    map[QStringLiteral("word")] = item.word;
    map[QStringLiteral("phonetic")] = item.phonetic;
    map[QStringLiteral("partOfSpeech")] = item.partOfSpeech;
    map[QStringLiteral("meaningVi")] = item.meaningVi;
    map[QStringLiteral("definitionEn")] = item.definitionEn;
    map[QStringLiteral("exampleSentence")] = item.exampleSentence;
    map[QStringLiteral("exampleTranslation")] = item.exampleTranslation;
    map[QStringLiteral("hintImage")] = item.hintImage;
    map[QStringLiteral("audioPath")] = item.audioPath;
    map[QStringLiteral("difficultyLevel")] = item.difficultyLevel;
    map[QStringLiteral("topicId")] = item.topicId;
    return map;
}

void WordListModel::addWord(const QVariantMap &item) {
    WordItem w;
    w.id = item.value(QStringLiteral("id"), m_allItems.size() + 1).toInt();
    w.word = item.value(QStringLiteral("word")).toString();
    w.phonetic = item.value(QStringLiteral("phonetic")).toString();
    w.partOfSpeech = item.value(QStringLiteral("partOfSpeech")).toString();
    w.meaningVi = item.value(QStringLiteral("meaningVi")).toString();
    w.definitionEn = item.value(QStringLiteral("definitionEn")).toString();
    w.exampleSentence = item.value(QStringLiteral("exampleSentence")).toString();
    w.exampleTranslation = item.value(QStringLiteral("exampleTranslation")).toString();
    w.hintImage = item.value(QStringLiteral("hintImage")).toString();
    w.audioPath = item.value(QStringLiteral("audioPath")).toString();
    w.difficultyLevel = item.value(QStringLiteral("difficultyLevel"), 1).toInt();
    w.topicId = item.value(QStringLiteral("topicId"), 1).toInt();

    m_allItems.append(w);

    beginInsertRows(QModelIndex(), m_displayItems.size(), m_displayItems.size());
    m_displayItems.append(w);
    endInsertRows();

    emit countChanged();
}

void WordListModel::filterByTopic(int topicId) {
    if (topicId <= 0) {
        resetFilter();
        return;
    }

    QVector<WordItem> filtered;
    for (const auto &item : m_allItems) {
        if (item.topicId == topicId) {
            filtered.append(item);
        }
    }
    updateDisplayItems(filtered);
}

void WordListModel::search(const QString &keyword) {
    const QString trimmed = keyword.trimmed().toLower();
    if (trimmed.isEmpty()) {
        resetFilter();
        return;
    }

    QVector<WordItem> filtered;
    for (const auto &item : m_allItems) {
        if (item.word.toLower().contains(trimmed) ||
            item.meaningVi.toLower().contains(trimmed) ||
            item.definitionEn.toLower().contains(trimmed)) {
            filtered.append(item);
        }
    }
    updateDisplayItems(filtered);
}

void WordListModel::resetFilter() {
    updateDisplayItems(m_allItems);
}

void WordListModel::clear() {
    beginResetModel();
    m_allItems.clear();
    m_displayItems.clear();
    endResetModel();
    emit countChanged();
}

void WordListModel::setWords(const QVector<WordItem> &words) {
    m_allItems = words;
    updateDisplayItems(words);
}

void WordListModel::updateDisplayItems(const QVector<WordItem> &newItems) {
    beginResetModel();
    m_displayItems = newItems;
    endResetModel();
    emit countChanged();
}
