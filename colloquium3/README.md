# Поведенческие шаблоны проектирования

## Определение поведенческих шаблонов проектирования

Поведенческие шаблоны проектирования определяют способы взаимодействия между объектами, распределяя обязанности между ними и описывая потоки управления. Эти шаблоны фокусируются на алгоритмах и назначении ответственности между объектами, повышая гибкость в выполнении коммуникации между ними.

В данном README рассмотрим шаблоны: Состояние, Посредник и Наблюдатель, анализируя их через призму инкапсуляции, принципа "разделяй и властвуй" и ортогональных стратегий, а также изучим влияние многопоточности на их применение.

## 1. Шаблон "Состояние" (State)

### Проблема
- Поведение объекта зависит от его состояния и должно изменяться во время выполнения
- Большое количество условных операторов управляет поведением в зависимости от состояния
- Усложнение кода при добавлении новых состояний
- Дублирование кода для одинаковых переходов состояний

### Решение
Позволить объекту изменять свое поведение при изменении внутреннего состояния, создавая впечатление, что объект изменил свой класс.

### Через призму инкапсуляции
Шаблон "Состояние" инкапсулирует состояния объекта в отдельных классах и делегирует им работу, зависящую от состояния. Это позволяет скрыть детали реализации каждого состояния и механизмы перехода между ними.

### Применение принципа "разделяй и властвуй"
Шаблон разделяет логику разных состояний на независимые классы, каждый из которых отвечает только за поведение, соответствующее конкретному состоянию.

### Ортогональность стратегий
Каждое состояние реализует собственную стратегию поведения независимо от других состояний. Контекст делегирует выполнение текущему состоянию, не зная деталей его реализации.

### Влияние многопоточности
В многопоточной среде переход между состояниями должен быть атомарным, чтобы избежать состояния гонки. Доступ к текущему состоянию контекста необходимо синхронизировать между потоками.

### Пример реализации

```cpp
#include <iostream>
#include <memory>
#include <mutex>
#include <string>

// Предварительное объявление классов
class Context;

// Абстрактный класс состояния
class State {
public:
    virtual ~State() = default;
    virtual void handle(Context& context) = 0;
    virtual std::string getName() const = 0;
};

// Контекст, управляемый состояниями
class Context {
private:
    std::shared_ptr<State> current_state_;
    std::mutex state_mutex_;  // Для многопоточной безопасности

public:
    Context(std::shared_ptr<State> initial_state) : current_state_(initial_state) {}

    void setState(std::shared_ptr<State> new_state) {
        std::lock_guard<std::mutex> lock(state_mutex_);
        std::cout << "Переход из состояния " << current_state_->getName() 
                  << " в состояние " << new_state->getName() << std::endl;
        current_state_ = new_state;
    }

    void request() {
        std::lock_guard<std::mutex> lock(state_mutex_);
        current_state_->handle(*this);
    }
};

// Конкретное состояние A
class ConcreteStateA : public State {
public:
    void handle(Context& context) override;
    std::string getName() const override { return "StateA"; }
};

// Конкретное состояние B
class ConcreteStateB : public State {
public:
    void handle(Context& context) override;
    std::string getName() const override { return "StateB"; }
};

// Конкретное состояние C
class ConcreteStateC : public State {
public:
    void handle(Context& context) override;
    std::string getName() const override { return "StateC"; }
};

// Реализация метода handle() для ConcreteStateA
void ConcreteStateA::handle(Context& context) {
    std::cout << "ConcreteStateA обрабатывает запрос." << std::endl;
    // Переход в состояние B
    context.setState(std::make_shared<ConcreteStateB>());
}

// Реализация метода handle() для ConcreteStateB
void ConcreteStateB::handle(Context& context) {
    std::cout << "ConcreteStateB обрабатывает запрос." << std::endl;
    // Переход в состояние C
    context.setState(std::make_shared<ConcreteStateC>());
}

// Реализация метода handle() для ConcreteStateC
void ConcreteStateC::handle(Context& context) {
    std::cout << "ConcreteStateC обрабатывает запрос." << std::endl;
    // Переход в состояние A
    context.setState(std::make_shared<ConcreteStateA>());
}

int main() {
    // Инициализация с начальным состоянием A
    auto initial_state = std::make_shared<ConcreteStateA>();
    Context context(initial_state);
    
    // Симулируем последовательность запросов
    context.request();  // A -> B
    context.request();  // B -> C
    context.request();  // C -> A
    
    return 0;
}
```

## 2. Шаблон "Посредник" (Mediator)

### Проблема
- Объекты системы имеют множество прямых связей между собой, что создает запутанную систему зависимостей
- Сложность повторного использования объектов из-за их тесной связанности
- Тесная связь между компонентами затрудняет модификацию системы
- Изменение одного компонента влечет за собой каскад изменений в других

### Решение
Определить объект-посредник, инкапсулирующий взаимодействие между множеством объектов, устраняя прямые зависимости между ними и позволяя им работать независимо.

### Через призму инкапсуляции
Шаблон "Посредник" инкапсулирует коллективное поведение объектов, скрывая детали их взаимодействия. Компоненты знают только о посреднике, а не друг о друге.

### Применение принципа "разделяй и властвуй"
Шаблон отделяет логику взаимодействия компонентов от их основной функциональности. Посредник берет на себя всю сложную логику координации, освобождая компоненты от этой ответственности.

### Ортогональность стратегий
Компоненты могут развиваться независимо от других компонентов системы, сохраняя связь только с посредником. Изменения в механизме взаимодействия не затрагивают компоненты.

### Влияние многопоточности
В многопоточной среде посредник должен обеспечивать синхронизацию взаимодействия между компонентами. Реализация посредника должна быть потокобезопасной, особенно при параллельной работе нескольких компонентов.

### Пример реализации

```cpp
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <mutex>

// Предварительное объявление классов
class Mediator;

// Абстрактный базовый класс для компонентов
class Colleague {
protected:
    std::shared_ptr<Mediator> mediator_;
    std::string name_;

public:
    Colleague(const std::string& name) : name_(name) {}
    virtual ~Colleague() = default;

    void setMediator(std::shared_ptr<Mediator> mediator) {
        mediator_ = mediator;
    }

    const std::string& getName() const {
        return name_;
    }

    virtual void send(const std::string& message) = 0;
    virtual void receive(const std::string& message, const std::string& from) = 0;
};

// Интерфейс посредника
class Mediator {
public:
    virtual ~Mediator() = default;
    virtual void addColleague(std::shared_ptr<Colleague> colleague) = 0;
    virtual void distribute(const std::string& message, const std::string& from) = 0;
};

// Конкретный посредник
class ConcreteMediator : public Mediator, public std::enable_shared_from_this<ConcreteMediator> {
private:
    std::vector<std::shared_ptr<Colleague>> colleagues_;
    std::mutex mutex_;  // Для многопоточной безопасности

public:
    void addColleague(std::shared_ptr<Colleague> colleague) override {
        std::lock_guard<std::mutex> lock(mutex_);
        colleague->setMediator(shared_from_this());
        colleagues_.push_back(colleague);
    }

    void distribute(const std::string& message, const std::string& from) override {
        std::lock_guard<std::mutex> lock(mutex_);
        std::cout << "Посредник распространяет сообщение от " << from << ": " << message << std::endl;
        
        for (auto& colleague : colleagues_) {
            if (colleague->getName() != from) {
                colleague->receive(message, from);
            }
        }
    }
};

// Конкретные коллеги
class ConcreteColleague : public Colleague {
public:
    ConcreteColleague(const std::string& name) : Colleague(name) {}

    void send(const std::string& message) override {
        std::cout << name_ << " отправляет сообщение: " << message << std::endl;
        mediator_->distribute(message, name_);
    }

    void receive(const std::string& message, const std::string& from) override {
        std::cout << name_ << " получает сообщение от " << from << ": " << message << std::endl;
    }
};

int main() {
    // Создаем посредника
    auto mediator = std::make_shared<ConcreteMediator>();
    
    // Создаем коллег
    auto colleague1 = std::make_shared<ConcreteColleague>("Компонент 1");
    auto colleague2 = std::make_shared<ConcreteColleague>("Компонент 2");
    auto colleague3 = std::make_shared<ConcreteColleague>("Компонент 3");
    
    // Регистрируем коллег у посредника
    mediator->addColleague(colleague1);
    mediator->addColleague(colleague2);
    mediator->addColleague(colleague3);
    
    // Отправляем сообщения
    colleague1->send("Привет от компонента 1");
    colleague2->send("Привет от компонента 2");
    
    return 0;
}
```

## 3. Шаблон "Наблюдатель" (Observer)

### Проблема
- Необходимо уведомлять множество объектов об изменениях в другом объекте
- Требуется слабая связанность между субъектом и наблюдателями
- Динамическое добавление/удаление подписчиков без изменения субъекта
- Нужно избежать опроса субъекта наблюдателями для обнаружения изменений

### Решение
Определить зависимость "один ко многим" между объектами так, чтобы при изменении состояния одного объекта (субъекта) все зависимые объекты (наблюдатели) автоматически оповещались и обновлялись.

### Через призму инкапсуляции
Шаблон "Наблюдатель" инкапсулирует список зависимых объектов и механизм их уведомления внутри субъекта, а наблюдатели инкапсулируют логику реакции на изменения.

### Применение принципа "разделяй и властвуй"
Шаблон разделяет субъект и его наблюдателей, позволяя им изменяться независимо. Субъект отвечает только за уведомление о изменениях, а наблюдатели — за реакцию на них.

### Ортогональность стратегий
Наблюдатели реализуют независимые стратегии реакции на изменения в субъекте, не влияя друг на друга. Субъект не зависит от конкретных реализаций наблюдателей.

### Влияние многопоточности
В многопоточной среде требуется синхронизация доступа к списку наблюдателей и процессу уведомления. Возникают проблемы с порядком уведомлений и возможными блокировками при длительной обработке обновлений наблюдателями.

### Пример реализации

```cpp
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <mutex>
#include <string>

// Интерфейс наблюдателя
class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(const std::string& message) = 0;
};

// Интерфейс субъекта
class Subject {
public:
    virtual ~Subject() = default;
    virtual void attach(std::shared_ptr<Observer> observer) = 0;
    virtual void detach(std::shared_ptr<Observer> observer) = 0;
    virtual void notify() = 0;
};

// Конкретный субъект
class ConcreteSubject : public Subject {
private:
    std::vector<std::shared_ptr<Observer>> observers_;
    std::string state_;
    std::mutex mutex_;  // Для многопоточной безопасности

public:
    void attach(std::shared_ptr<Observer> observer) override {
        std::lock_guard<std::mutex> lock(mutex_);
        observers_.push_back(observer);
    }

    void detach(std::shared_ptr<Observer> observer) override {
        std::lock_guard<std::mutex> lock(mutex_);
        observers_.erase(
            std::remove_if(observers_.begin(), observers_.end(),
                [&](std::shared_ptr<Observer> o) { return o == observer; }),
            observers_.end()
        );
    }

    void notify() override {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& observer : observers_) {
            observer->update(state_);
        }
    }

    void setState(const std::string& state) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            state_ = state;
            std::cout << "Субъект: Состояние изменено на " << state_ << std::endl;
        }
        notify();
    }
};

// Конкретный наблюдатель
class ConcreteObserver : public Observer {
private:
    std::string name_;
    std::string observed_state_;

public:
    ConcreteObserver(const std::string& name) : name_(name) {}

    void update(const std::string& message) override {
        observed_state_ = message;
        std::cout << "Наблюдатель " << name_ << " получил обновление: " << observed_state_ << std::endl;
    }
};

int main() {
    // Создаем субъект
    auto subject = std::make_shared<ConcreteSubject>();
    
    // Создаем наблюдателей
    auto observer1 = std::make_shared<ConcreteObserver>("Наблюдатель 1");
    auto observer2 = std::make_shared<ConcreteObserver>("Наблюдатель 2");
    auto observer3 = std::make_shared<ConcreteObserver>("Наблюдатель 3");
    
    // Подписываем наблюдателей на уведомления
    subject->attach(observer1);
    subject->attach(observer2);
    subject->attach(observer3);
    
    // Изменяем состояние субъекта
    subject->setState("Первое обновление");
    
    // Отписываем второго наблюдателя
    subject->detach(observer2);
    
    // Еще раз изменяем состояние
    subject->setState("Второе обновление");
    
    return 0;
}
```

### Пример использования Observer в Qt

В библиотеке Qt шаблон "Наблюдатель" реализован через механизм сигналов и слотов, который является одной из ключевых особенностей Qt:

```cpp
#include <QApplication>
#include <QObject>
#include <QDebug>
#include <QTimer>

// Субъект (издатель)
class DataSource : public QObject {
    Q_OBJECT
private:
    int value_;

public:
    explicit DataSource(QObject* parent = nullptr) : QObject(parent), value_(0) {}

    int value() const { return value_; }

    void setValue(int value) {
        if (value_ != value) {
            value_ = value;
            emit valueChanged(value_);  // Уведомление наблюдателей
            qDebug() << "DataSource: значение изменено на" << value_;
        }
    }

signals:
    // Сигнал, который автоматически уведомляет наблюдателей
    void valueChanged(int newValue);
};

// Наблюдатель 1
class DataDisplay : public QObject {
    Q_OBJECT
public:
    explicit DataDisplay(QObject* parent = nullptr) : QObject(parent) {}

public slots:
    // Слот, который вызывается при получении сигнала
    void onValueChanged(int value) {
        qDebug() << "DataDisplay: получено новое значение:" << value;
    }
};

// Наблюдатель 2
class Logger : public QObject {
    Q_OBJECT
public:
    explicit Logger(QObject* parent = nullptr) : QObject(parent) {}

public slots:
    void logValue(int value) {
        qDebug() << "Logger: записано значение:" << value;
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Создаем субъект
    DataSource dataSource;
    
    // Создаем наблюдателей
    DataDisplay display;
    Logger logger;
    
    // Подключаем сигналы к слотам (подписываем наблюдателей)
    QObject::connect(&dataSource, &DataSource::valueChanged, &display, &DataDisplay::onValueChanged);
    QObject::connect(&dataSource, &DataSource::valueChanged, &logger, &Logger::logValue);
    
    // Изменяем значение в источнике данных
    dataSource.setValue(42);
    
    // Отключаем логгер (отписываем наблюдателя)
    QObject::disconnect(&dataSource, &DataSource::valueChanged, &logger, &Logger::logValue);
    
    // Изменяем значение еще раз
    dataSource.setValue(100);
    
    // Используем таймер для завершения приложения
    QTimer::singleShot(0, &app, &QApplication::quit);
    
    return app.exec();
}

#include "main.moc"  // Необходимо для метаобъектной компиляции
```

В этом примере:
1. `DataSource` — субъект, который отправляет сигналы при изменении своего состояния
2. `DataDisplay` и `Logger` — наблюдатели, которые реагируют на изменения через слоты
3. Механизм `connect()` создает связь между сигналами и слотами
4. Система сигналов и слотов Qt обеспечивает потокобезопасное взаимодействие через очереди событий

Qt автоматически обрабатывает многие аспекты многопоточности при использовании сигналов и слотов между потоками, включая синхронизацию и очередность событий.

## Программный дизайн и влияние многопоточности

### Что такое дизайн программного обеспечения

Дизайн программного обеспечения — это процесс определения архитектуры, компонентов, интерфейсов и других характеристик системы или компонента. Это планирование решения программной задачи до этапа написания кода.

Программный дизайн включает в себя:

1. **Концептуальное проектирование**: определение основных абстракций и их взаимоотношений
2. **Логическое проектирование**: определение структуры данных, взаимодействия между компонентами
3. **Физическое проектирование**: реализация компонентов, выбор технологий и платформ
4. **Проектирование интерфейсов**: определение способов взаимодействия между компонентами и с внешними системами

Хороший дизайн ПО характеризуется:
- Модульностью
- Слабой связанностью
- Высокой связностью внутри модулей
- Масштабируемостью
- Тестируемостью
- Повторным использованием кода

### Влияние многопоточности на дизайн программного обеспечения

Многопоточность существенно влияет на дизайн ПО, добавляя новые измерения сложности и требуя пересмотра многих шаблонов и принципов проектирования:

#### 1. Влияние на объектно-ориентированные принципы

**Инкапсуляция**:
- Необходимость защиты состояния объектов при параллельном доступе
- Внедрение потокобезопасных механизмов доступа к данным
- Использование неизменяемых (immutable) объектов

**Наследование**:
- Проблемы с параллельным доступом к наследуемым полям
- Сложность обеспечения потокобезопасности при наследовании
- Тенденция к использованию композиции вместо наследования

**Полиморфизм**:
- Безопасность динамической диспетчеризации в многопоточной среде
- Синхронизация при работе с полиморфными объектами

#### 2. Влияние на шаблоны проектирования

**Адаптация шаблонов**:
- Необходимость в потокобезопасных версиях классических шаблонов
- Специальные шаблоны для многопоточной среды (Monitor, Thread Pool, Active Object)

**Усложнение шаблонов**:
- Добавление механизмов синхронизации в существующие шаблоны
- Дополнительные проверки безопасности в критических секциях

#### 3. Новые аспекты проектирования

**Стратегии синхронизации**:
- Выбор между блокировками, атомарными операциями, неблокирующими структурами данных
- Баланс между безопасностью и производительностью

**Модели параллелизма**:
- Параллелизм данных vs. параллелизм задач
- Разделение домена на независимые компоненты

**Управление состоянием**:
- Минимизация и изоляция изменяемого состояния
- Предпочтение функциональных подходов и чистых функций

#### 4. Архитектурные изменения

**Распределение ответственности**:
- Выделение потокобезопасных и потоконебезопасных частей системы
- Явное определение границ параллелизма

**Асинхронное проектирование**:
- Переход от синхронных к асинхронным моделям взаимодействия
- Использование очередей сообщений и событийно-ориентированных архитектур

**Масштабируемость**:
- Проектирование для горизонтального масштабирования
- Анализ узких мест параллельной производительности

#### 5. Практические соображения

**Отладка и тестирование**:
- Необходимость новых подходов к тестированию многопоточного кода
- Сложность воспроизведения и отладки ошибок многопоточности

**Документация**:
- Явное документирование потокобезопасности компонентов
- Описание предположений о параллельном использовании

**Управление сложностью**:
- Применение более строгих методологий проектирования
- Формальная верификация критических участков кода

В целом, многопоточность требует более дисциплинированного подхода к дизайну ПО, с тщательным анализом взаимодействий между компонентами и особым вниманием к управлению состоянием. Проектирование становится более формальным и часто включает элементы функционального программирования для минимизации проблем, связанных с параллельным доступом к изменяемому состоянию.
