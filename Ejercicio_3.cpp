#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <cmath>
#include <utility>

// Clase abstracta Expression
class Expression {
public:
    virtual ~Expression() = default;
    virtual double evaluate(double x) const = 0;
    virtual std::shared_ptr<Expression> derivative() const = 0;
    virtual std::string toString() const = 0;

    virtual bool isConstant() const { return false; }
    virtual double getConstantValue() const { return 0.0; }
};

// Clase Constant
class Constant : public Expression {
private:
    double value;
public:
    explicit Constant(double val) : value(val) {}

    double evaluate(double /*x*/) const override {
        return value;
    }

    std::shared_ptr<Expression> derivative() const override {
        return std::make_shared<Constant>(0.0);
    }

    std::string toString() const override {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

    bool isConstant() const override {
        return true;
    }

    double getConstantValue() const override {
        return value;
    }
};

// Clase Variable
class Variable : public Expression {
private:
    std::string name;
public:
    explicit Variable(std::string name = "x") : name(std::move(name)) {}

    double evaluate(double x) const override {
        return x;
    }

    std::shared_ptr<Expression> derivative() const override {
        return std::make_shared<Constant>(1.0);
    }

    std::string toString() const override {
        return name;
    }
};

// Clase abstracta BinaryOperation
class BinaryOperation : public Expression {
protected:
    std::shared_ptr<Expression> left;
    std::shared_ptr<Expression> right;
public:
    BinaryOperation(std::shared_ptr<Expression> left, std::shared_ptr<Expression> right)
        : left(std::move(left)), right(std::move(right)) {}
};

// Clase abstracta UnaryFunction
class UnaryFunction : public Expression {
protected:
    std::shared_ptr<Expression> argument;
public:
    explicit UnaryFunction(std::shared_ptr<Expression> arg)
        : argument(std::move(arg)) {}
};

// Logaritmo natural: ln(u)
class Log : public UnaryFunction {
public:
    explicit Log(std::shared_ptr<Expression> arg) : UnaryFunction(std::move(arg)) {}

    double evaluate(double x) const override {
        return std::log(argument->evaluate(x));
    }

    std::shared_ptr<Expression> derivative() const override;

    std::string toString() const override {
        return "ln(" + argument->toString() + ")";
    }
};

// Suma: (u + v)
class Add : public BinaryOperation {
public:
    Add(std::shared_ptr<Expression> left, std::shared_ptr<Expression> right)
        : BinaryOperation(std::move(left), std::move(right)) {}

    double evaluate(double x) const override {
        return left->evaluate(x) + right->evaluate(x);
    }

    std::shared_ptr<Expression> derivative() const override {
        return std::make_shared<Add>(left->derivative(), right->derivative());
    }

    std::string toString() const override {
        return "(" + left->toString() + " + " + right->toString() + ")";
    }
};

// Resta: (u - v)
class Subtract : public BinaryOperation {
public:
    Subtract(std::shared_ptr<Expression> left, std::shared_ptr<Expression> right)
        : BinaryOperation(std::move(left), std::move(right)) {}

    double evaluate(double x) const override {
        return left->evaluate(x) - right->evaluate(x);
    }

    std::shared_ptr<Expression> derivative() const override {
        return std::make_shared<Subtract>(left->derivative(), right->derivative());
    }

    std::string toString() const override {
        return "(" + left->toString() + " - " + right->toString() + ")";
    }
};

// Multiplicacion: u * v
class Multiply : public BinaryOperation {
public:
    Multiply(std::shared_ptr<Expression> left, std::shared_ptr<Expression> right)
        : BinaryOperation(std::move(left), std::move(right)) {}

    double evaluate(double x) const override {
        return left->evaluate(x) * right->evaluate(x);
    }

    std::shared_ptr<Expression> derivative() const override {
        return std::make_shared<Add>(
            std::make_shared<Multiply>(left->derivative(), right),
            std::make_shared<Multiply>(left, right->derivative())
        );
    }

    std::string toString() const override {
        return "(" + left->toString() + " * " + right->toString() + ")";
    }
};

// Potencia: u^v
class Power : public BinaryOperation {
public:
    Power(std::shared_ptr<Expression> base, std::shared_ptr<Expression> exp)
        : BinaryOperation(std::move(base), std::move(exp)) {}

    double evaluate(double x) const override {
        return std::pow(left->evaluate(x), right->evaluate(x));
    }

    std::shared_ptr<Expression> derivative() const override;

    std::string toString() const override {
        return "(" + left->toString() + ")^(" + right->toString() + ")";
    }
};

// Division: (u / v)
class Divide : public BinaryOperation {
public:
    Divide(std::shared_ptr<Expression> left, std::shared_ptr<Expression> right)
        : BinaryOperation(std::move(left), std::move(right)) {}

    double evaluate(double x) const override {
        return left->evaluate(x) / right->evaluate(x);
    }

    std::shared_ptr<Expression> derivative() const override {
        return std::make_shared<Divide>(
            std::make_shared<Subtract>(
                std::make_shared<Multiply>(left->derivative(), right),
                std::make_shared<Multiply>(left, right->derivative())
            ),
            std::make_shared<Power>(right, std::make_shared<Constant>(2.0))
        );
    }

    std::string toString() const override {
        return "(" + left->toString() + " / " + right->toString() + ")";
    }
};

inline std::shared_ptr<Expression> Power::derivative() const {
    if (right->isConstant()) {
        double n = right->getConstantValue();
        if (n == 0.0) {
            return std::make_shared<Constant>(0.0);
        }
        if (n == 1.0) {
            return left->derivative();
        }
        return std::make_shared<Multiply>(
            std::make_shared<Multiply>(
                right,
                std::make_shared<Power>(left, std::make_shared<Constant>(n - 1.0))
            ),
            left->derivative()
        );
    }

    // d(u^v) = u^v * (v' * ln(u) + v * u' / u), for u > 0.
    auto exponentTerm = std::make_shared<Multiply>(
        right->derivative(), std::make_shared<Log>(left));
    auto baseTerm = std::make_shared<Divide>(
        std::make_shared<Multiply>(right, left->derivative()), left);
    return std::make_shared<Multiply>(
        std::make_shared<Power>(left, right),
        std::make_shared<Add>(exponentTerm, baseTerm));
}

inline std::shared_ptr<Expression> Log::derivative() const {
    return std::make_shared<Divide>(argument->derivative(), argument);
}

class Cos;

// Seno: sin(u)
class Sin : public UnaryFunction {
public:
    explicit Sin(std::shared_ptr<Expression> arg) : UnaryFunction(std::move(arg)) {}

    double evaluate(double x) const override {
        return std::sin(argument->evaluate(x));
    }

    std::shared_ptr<Expression> derivative() const override;

    std::string toString() const override {
        return "sin(" + argument->toString() + ")";
    }
};

// Coseno: cos(u)
class Cos : public UnaryFunction {
public:
    explicit Cos(std::shared_ptr<Expression> arg) : UnaryFunction(std::move(arg)) {}

    double evaluate(double x) const override {
        return std::cos(argument->evaluate(x));
    }

    std::shared_ptr<Expression> derivative() const override {
        return std::make_shared<Multiply>(
            std::make_shared<Multiply>(
                std::make_shared<Constant>(-1.0),
                std::make_shared<Sin>(argument)
            ),
            argument->derivative()
        );
    }

    std::string toString() const override {
        return "cos(" + argument->toString() + ")";
    }
};

inline std::shared_ptr<Expression> Sin::derivative() const {
    return std::make_shared<Multiply>(
        std::make_shared<Cos>(argument),
        argument->derivative()
    );
}

int main() {
    auto x = std::make_shared<Variable>();
    auto expr =
    std::make_shared<Multiply>(
        std::make_shared<Add>(
            std::make_shared<Power>(
                x,
                std::make_shared<Constant>(2)
            ),
            std::make_shared<Multiply>(
                std::make_shared<Constant>(3),
                x
            )
        ),
        std::make_shared<Sin>(x)
    );
    auto deriv = expr->derivative();
    std::cout << "f(x) = " << expr->toString() << std::endl;
    std::cout << "f'(x) = " << deriv->toString() << std::endl;
    std::cout << "f(2) = " << expr->evaluate(2) << std::endl;
}
