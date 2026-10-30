#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <memory>

using namespace std;

class IdGenerator {
  public:
    static long long generate() {
      return chrono::system_clock::now().time_since_epoch().count();
    }
};

// PATRON CREACIONAL: BUILDER
// Problem tiene muchos atributos, su constructor es privado y solo ProblemBuilder (que tiene a Problem como class friend) puede crearlo, el builder arma el objeto paso a paso y build() devuelve el Problem final, evitando constructores con Problem(...)
class ProblemBuilder;

class Problem {
  private:
    long long id;
    string title, statement, input, output;
    int time_limit;

    Problem(const string& t, const string& s, const string& i, const string& o, int tl)
      : id{IdGenerator::generate()}, title{t}, statement{s}, input{i}, output{o}, time_limit{tl} {}
    friend class ProblemBuilder;

  public:
    long long get_id() const {
      return id;
    }
    string get_title() const {
      return title;
    }
};

class ProblemBuilder {
  private:
    string title = "Untitled", statement, input, output;
    int time_limit = 1; // segundos

  public:
    ProblemBuilder& set_title(const string& t) {
      title = t;
      return *this;
    }

    ProblemBuilder& set_statement(const string& s) {
      statement = s;
      return *this;
    }

    ProblemBuilder& set_input(const string& i) {
      input = i;return *this;
    }

    ProblemBuilder& set_output(const string& o) {
      output = o;
      return *this;
    }

    ProblemBuilder& set_time_limit(int tl) {
      time_limit = tl;
      return *this;
    }

    Problem build() const {
      return Problem(title, statement, input, output, time_limit);
    }
};

// PATRON ESTRUCTURAL: DECORATOR
// Judge es el Component, ServerJudge es el ConcreteComponent, LoggingJudge es el Decorator, implementa Judge, envuelve a otro Judge y agrega logging antes/despues de delegar, sin modificar ServerJudge.
class Judge {
  public:
    virtual ~Judge() = default;
    virtual string judge(long long problem_id, const string& solution) = 0;
};

class ServerJudge : public Judge {
  public:
    string judge(long long problem_id, const string& solution) override {
      return "accepted";
    }
};

class LoggingJudge : public Judge {
  private:
    Judge& inner; // Judge decorado

  public:
    explicit LoggingJudge(Judge& j) : inner{j} {}

    string judge(long long problem_id, const string& solution) override {
      cout << "[LOG] Judging problem " << problem_id << " (" << solution.size() << " bytes)\n";
      string verdict = inner.judge(problem_id, solution);
      cout << "[LOG] Verdict: " << verdict << '\n';
      return verdict;
    }
};

// PATRON DE COMPORTAMIENTO: OBSERVER
// ServerRepository es el Subject, mantiene una lista de Observers y los notifica cuando se sube un problema nuevo, Contestant es un ConcreteObserver, implementa update() y reacciona al evento, el repositorio no conoce las clases concretas, solo la interfaz Observer.
class Observer {
  public:
    virtual ~Observer() = default;
    virtual void update(const string& message) = 0;
};

class ServerRepository {
  private:
    map<long long, Problem> repository;
    vector<Observer*> observers;

  public:
    void subscribe(Observer* o) {
      observers.push_back(o);
    }

    void upload(Problem& problem) {
      repository.emplace(problem.get_id(), problem);
      for (Observer* o : observers) { // notificar a todos los observers concretos
        o->update("New problem available: " + problem.get_title());
      }
    }
};

// Clases de la actividad 8
class User {
  private:
    string username;

  public:
    User(const string& _username) : username{_username} {}
    virtual ~User() = default;

    string get_username() {
      return username;
    }
};

class Contestant : public User, public Observer {
  public:
    explicit Contestant(const string& username) : User(username) {}

    void update(const string& message) override { // implementa de la interfaz de Observer
      cout << "[" << get_username() << " notified] " << message << '\n';
    }

    void submit_problem(long long problem_id, const string& solution, Judge& srv_judge) {
      cout << get_username() << " is submitting problem " << problem_id << '\n';
      string verdict = srv_judge.judge(problem_id, solution);
      cout << "Verdict received: " << verdict << '\n';
    }
};

class Staff : public User {
  public:
    explicit Staff(const string& username) : User(username) {}

    void upload_problem(Problem& problem, ServerRepository& srv_repo) {
      cout << get_username() << " is uploading the problem..." << '\n';
      srv_repo.upload(problem);
      cout << "Upload complete" << '\n';
    }
};

int main() {
  ServerJudge srv_judge;
  LoggingJudge logged_judge(srv_judge); // Decorator sobre ServerJudge
  ServerRepository srv_repo;

  auto staff = make_unique<Staff>("staff01");
  auto contestant = make_unique<Contestant>("zebnel");
  srv_repo.subscribe(contestant.get()); // Observer se suscribe

  // Builder, el problema se arma paso a paso
  Problem problem = ProblemBuilder()
    .set_title("A + B")
    .set_statement("Print a + b")
    .set_input("2 3")
    .set_output("5")
    .set_time_limit(2)
    .build();

  staff->upload_problem(problem, srv_repo); // notificacion al Observer
  contestant->submit_problem(problem.get_id(), "int main() { /* a + b */ }", logged_judge);

  return 0;
}
