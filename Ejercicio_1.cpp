#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

class Account {
    std::string id;
    double balance;
    mutable std::mutex mtx;
public:
    Account(std::string id, double initial) : id(std::move(id)), balance(initial) {}
    Account(const Account&) = delete;
    Account& operator=(const Account&) = delete;

    double getBalance() const {
        std::lock_guard<std::mutex> lock(mtx);
        return balance;
    }

    friend class Transaction;
};

class Transaction {
public:
    enum class Type { Deposit, Withdraw };
private:
    Account* account;
    Type type;
    double amount;
public:
    Transaction(Account* acc, Type t, double amt) : account(acc), type(t), amount(amt) {}

    bool execute() {
        if (!account || amount <= 0) return false;
        std::lock_guard<std::mutex> lock(account->mtx);
        if (type == Type::Deposit) {
            account->balance += amount;
        } else {
            if (account->balance < amount) return false;
            account->balance -= amount;
        }
        return true;
    }
};

class TransactionManager {
private:
    static TransactionManager instance;

    TransactionManager() = default;
public:
    TransactionManager(const TransactionManager&) = delete;
    TransactionManager& operator=(const TransactionManager&) = delete;

    static TransactionManager& getInstance() { return instance; }

    bool deposit(Account* acc, double amount) {
        return Transaction(acc, Transaction::Type::Deposit, amount).execute();
    }

    bool withdraw(Account* acc, double amount) {
        return Transaction(acc, Transaction::Type::Withdraw, amount).execute();
    }
};

TransactionManager TransactionManager::instance;

int main() {
    auto& manager = TransactionManager::getInstance();
    Account account("A1", 1000);

    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&]() {
            manager.deposit(&account, 100);
            manager.withdraw(&account, 50);
        });
    }
    for (auto& t : threads) t.join();

    std::cout << "Balance final: " << account.getBalance() << std::endl;
    std::cout << "Direccion manager: " << &TransactionManager::getInstance() << std::endl;
}