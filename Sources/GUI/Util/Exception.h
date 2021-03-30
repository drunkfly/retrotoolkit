#ifndef GUI_UTIL_EXCEPTION_H
#define GUI_UTIL_EXCEPTION_H

#include "GUI/Common.h"

#define TRY \
    try { \
        try

#define CATCH(e) \
          catch (const Exception& e) { \
            throw; \
        } catch (const std::bad_alloc& e) { \
            throw Exception(e); \
        } catch (const std::exception& e) { \
            throw Exception(e); \
        } catch (...) { \
            throw Exception(); \
        } \
    } catch (const Exception& e)

class QWidget;

class Exception
{
public:
    Exception();
    explicit Exception(const std::bad_alloc&);
    explicit Exception(const std::exception& e);
    explicit Exception(const QString& message);
    virtual ~Exception();

    const QString& message() const { return mMessage; }

    void show(QWidget* parent) const;

private:
    QString mMessage;
};

#endif
