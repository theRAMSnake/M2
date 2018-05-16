#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <type_traits>

template <typename T, typename D = T>
class Singleton
{
    friend D;
    static_assert(std::is_base_of<T, D>::value, "T should be a base type for D");

public:
    static T& instance();

private:
    Singleton() = default;
    ~Singleton() = default;
    Singleton( const Singleton& ) = delete;
    Singleton& operator=( const Singleton& ) = delete;
};

template <typename T, typename D>
T& Singleton<T, D>::instance()
{
    static D inst;
    return inst;
}

enum class MessageType
{
    Info,
    Warning,
    Error
};

struct Message
{
    QString str;
    MessageType type;
};

class Logger : public QObject, public Singleton<Logger>
{
    Q_OBJECT
public:
    void log(const QString& msg);

signals:
    void onNewMessage(const Message message);

public slots:
};

#endif // LOGGER_H
