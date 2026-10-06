/**
 * @file client.cpp
 * @brief Клиентский код: проверка класса Polynomial, подключённого как библиотека.
 * @details Это **один и тот же** файл для обеих тестовых программ лабораторной
 * работы №5: он компилируется и линкуется один раз со статической библиотекой
 * (test_static.exe) и один раз с динамической (test_dynamic.exe). Код пользуется
 * только открытым интерфейсом из Polynomial.h и не знает, какая библиотека
 * под ним подключена, — поэтому у обеих программ вывод должен быть одинаковым.
 *
 * Проверяется то, что важно именно для библиотеки: обычные операции класса,
 * статические счётчики (они живут внутри библиотеки) и исключения, которые
 * бросает код библиотеки, а ловит клиент.
 * @author Vareshka86
 * @date 2026-10-06
 * @version 0.1
 */

#include "Polynomial.h"

#include <cmath>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>

/// Счётчики проверок и вспомогательные функции, видимые только внутри этого файла.
namespace
{

/// Сколько проверок выполнено.
int g_total = 0;

/// Сколько проверок прошло успешно.
int g_passed = 0;

/**
 * @brief Засчитывает одну проверку и печатает её результат.
 * @param description Что проверялось.
 * @param passed      true — ожидание совпало с результатом.
 */
void report(const std::string& description, bool passed)
{
    ++g_total;
    if (passed)
    {
        ++g_passed;
    }
    std::cout << (passed ? "[OK]     " : "[ОШИБКА] ") << description << '\n';
}

/**
 * @brief Выводит заголовок группы проверок.
 * @param title Название группы.
 */
void section(const std::string& title)
{
    std::cout << "\n--- " << title << " ---\n";
}

/**
 * @brief Сравнивает дробные числа с учётом погрешности вычислений.
 * @param a Первое число.
 * @param b Второе число.
 * @return true, если числа различаются меньше чем на 10⁻⁹.
 */
bool near(double a, double b)
{
    return std::fabs(a - b) < 1e-9;
}

} // namespace

/**
 * @brief Точка входа клиента: запускает проверки и печатает итог.
 * @return 0 — все проверки пройдены, 1 — есть ошибки.
 */
int main()
{
    std::cout << "Клиент библиотеки lab3: проверка класса Polynomial\n";

    section("1. Создание и значения");
    {
        const int before = Polynomial::getExistingCount();
        Polynomial p(1.0, -3.0, 2.0);
        report("многочлен x² - 3x + 2 записан в виде «" + p.toString() + "»", p.toString() == "x² - 3x + 2");
        report("P(0) = 2 и P(3) = 2 (схема Горнера)", near(p.valueAt(0.0), 2.0) && near(p.valueAt(3.0), 2.0));
        report("P(1) = 0 и P(2) = 0 — корни",
               near(p.valueAt(1.0), 0.0) && near(p.valueAt(2.0), 0.0));
        Polynomial zero;
        report("конструктор по умолчанию даёт нулевой многочлен «" + zero.toString() + "»", zero.toString() == "0");
        report("счётчик существующих многочленов (статическое поле библиотеки) вырос на 2",
               Polynomial::getExistingCount() == before + 2);
    }
    report("после выхода из блока счётчик вернулся: деструктор сработал в библиотеке",
           Polynomial::getExistingCount() == 0);

    section("2. Корни и счётчик поисков");
    {
        const int searches = Polynomial::getRootSearchCount();
        double x1 = 0.0;
        double x2 = 0.0;
        const int two = Polynomial(1.0, -3.0, 2.0).findRoots(x1, x2);
        report("x² - 3x + 2: два корня 1 и 2", two == 2 && near(x1, 1.0) && near(x2, 2.0));
        const int one = Polynomial(1.0, 2.0, 1.0).findRoots(x1, x2);
        report("x² + 2x + 1: один корень -1", one == 1 && near(x1, -1.0));
        const int none = Polynomial(1.0, 0.0, 1.0).findRoots(x1, x2);
        report("x² + 1: действительных корней нет", none == 0);
        const int any = Polynomial().findRoots(x1, x2);
        report("нулевой многочлен: корнем является любое число", any == Polynomial::INFINITE_ROOTS);
        report("статический счётчик поисков корней вырос на 4",
               Polynomial::getRootSearchCount() == searches + 4);
    }

    section("3. Операции");
    {
        const Polynomial p(2.0, 3.0, 1.0);
        const Polynomial q(1.0, -1.0, 4.0);
        report("p + q = «" + (p + q).toString() + "»", (p + q).toString() == "3x² + 2x + 5");
        report("p - q = «" + (p - q).toString() + "»", (p - q).toString() == "x² + 4x - 3");
        report("p * 2 = «" + (p * 2.0).toString() + "» и 2 * p дают одно и то же",
               (p * 2.0).toString() == "4x² + 6x + 2" && (2.0 * p).toString() == (p * 2.0).toString());
        report("p / 2 = «" + (p / 2.0).toString() + "»", (p / 2.0).toString() == "x² + 1.5x + 0.5");
        Polynomial r(p);
        ++r;
        Polynomial before = r++;
        report("++r и r++: до последнего увеличения было «" + before.toString() + "», стало «" + r.toString() + "»",
               before.toString() == "3x² + 4x + 2" && r.toString() == "4x² + 5x + 3");
        std::ostringstream out;
        out << p;
        report("вывод в поток через operator<<: «" + out.str() + "»", out.str() == "2x² + 3x + 1");
    }

    section("4. Сравнения в точке x₀");
    {
        Polynomial::setComparePoint(2.0);
        const Polynomial p(1.0, 0.0, 0.0); // P(2) = 4
        const Polynomial q(0.0, 3.0, 0.0); // Q(2) = 6
        report("точка сравнения x₀ = 2 (статическое поле библиотеки)", near(Polynomial::getComparePoint(), 2.0));
        report("P(2) = 4 < Q(2) = 6", p < q && q > p && p <= q && q >= p && p != q && !(p == q));
        Polynomial::setComparePoint(0.0);
    }

    section("5. Исключения, брошенные библиотекой");
    {
        bool invalidArgument = false;
        try
        {
            Polynomial bad(std::numeric_limits<double>::infinity(), 0.0, 0.0);
        }
        catch (const std::invalid_argument&)
        {
            invalidArgument = true;
        }
        report("бесконечный коэффициент: клиент поймал std::invalid_argument", invalidArgument);

        bool divideByZero = false;
        Polynomial p(1.0, 2.0, 3.0);
        try
        {
            p /= 0.0;
        }
        catch (const std::invalid_argument&)
        {
            divideByZero = true;
        }
        report("деление на 0: std::invalid_argument, многочлен прежний «" + p.toString() + "»",
               divideByZero && p.toString() == "x² + 2x + 3");

        bool overflow = false;
        Polynomial big(1e308, 0.0, 0.0);
        try
        {
            big *= 10.0;
        }
        catch (const std::overflow_error&)
        {
            overflow = true;
        }
        report("переполнение: std::overflow_error, коэффициент прежний", overflow && near(big.getA(), 1e308));

        report("после исключений счётчик многочленов верный: остались p и big",
               Polynomial::getExistingCount() == 2);
    }

    std::cout << "\nИтог: проверок пройдено " << g_passed << " из " << g_total << '\n';
    return g_passed == g_total ? 0 : 1;
}
