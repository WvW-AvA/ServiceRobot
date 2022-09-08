% ASP file for @Home Simulation 2012
% The file is running for clingo or gringo + clasp or iclingo
% vstate.lp and vaction.lp are needed for initial state and the action.
% forcons.lp is also needed to check whether the constraint is satisfied by the next state.
% Written by Jianmin JI. jianmin@ustc.edu.cn
% 2012-10-22

#base.

%% Init
h(closed(B), 0) :- not h(opened(B), 0), obj(B), container(B).


%% Action restructions.
% list of actions.
action_of(move(X)) :- loc(X).
action_of(pickup(A)) :- obj(A), small(A).
action_of(putdown(A)) :- obj(A), small(A).
action_of(toplate(A)) :- obj(A), small(A).
action_of(fromplate(A)) :- obj(A), small(A).
action_of(open(A)) :- obj(A), container(A).
action_of(close(A)) :- obj(A), container(A).
action_of(putin(A, B)) :- obj(A), small(A), obj(B), container(B).
action_of(takeout(A, B)) :- obj(A), small(A), obj(B), container(B).
%action_of(askloc(A)) :- obj(A).
%action_of(askloc(0)).
%action_of(sense).


#cumulative t.

1{occurs(A,t):action_of(A)}1.


%% Descriptions of actions.
% action move(X)
h(at(0, X), t) :- occurs(move(X), t), loc(X).
h(change(0), t) :- occurs(move(X), t), loc(X).
:- occurs(move(X), t), h(at(0, X), t-1), loc(X).

% action pickup(A)
h(hold(A), t) :- occurs(pickup(A), t), obj(A), small(A).
h(changeh(0), t) :- occurs(pickup(A), t), obj(A), small(A).
:- occurs(pickup(A), t), not h(hold(0), t-1), obj(A), small(A).
:- occurs(pickup(A), t), h(plate(A), t-1), obj(A), small(A).
:- occurs(pickup(A), t), not h(sameloc(A), t-1), obj(A), small(A).

% action putdown(A)
h(hold(0), t) :- occurs(putdown(A), t), obj(A), small(A).
h(changeh(A), t) :- occurs(putdown(A), t), obj(A), small(A).
:- occurs(putdown(A), t), not h(hold(A), t-1), obj(A), small(A).

% action toplate(A)
h(plate(A), t) :- occurs(toplate(A), t), obj(A), small(A).
h(hold(0), t) :- occurs(toplate(A), t), obj(A), small(A).
h(changeh(A), t) :- occurs(toplate(A), t), obj(A), small(A).
h(changep(0), t) :- occurs(toplate(A), t), obj(A), small(A).
:- occurs(toplate(A), t), not h(hold(A), t-1), obj(A), small(A).
:- occurs(toplate(A), t), not h(plate(0), t-1), obj(A), small(A).

% action fromplate(A)
h(hold(A), t) :- occurs(fromplate(A), t), obj(A), small(A).
h(plate(0), t) :- occurs(fromplate(A), t), obj(A), small(A).
h(changeh(0), t) :- occurs(fromplate(A), t), obj(A), small(A).
h(changep(A), t) :- occurs(fromplate(A), t), obj(A), small(A).
:- occurs(fromplate(A), t), not h(plate(A), t-1), obj(A), small(A).
:- occurs(fromplate(A), t), not h(hold(0), t-1), obj(A), small(A).

% action open(A) 
h(opened(A), t) :- occurs(open(A), t), obj(A), container(A).
:- occurs(open(A), t), not h(hold(0), t-1), obj(A), container(A).
:- occurs(open(A), t), not h(closed(A), t-1), obj(A), container(A).
:- occurs(open(A), t), not h(sameloc(A), t-1), obj(A), container(A).

% action close(A)
h(closed(A), t) :- occurs(close(A), t), obj(A), container(A).
:- occurs(close(A), t), not h(hold(0), t-1), obj(A), container(A).
:- occurs(close(A), t), not h(sameloc(A), t-1), obj(A), container(A).
:- occurs(close(A), t), not h(opened(A), t-1), obj(A), container(A).

% action putin(A, B) 
h(inside(A, B), t) :- occurs(putin(A, B), t), obj(A), small(A), obj(B), container(B).
h(hold(0), t) :- occurs(putin(A, B), t), obj(A), small(A), obj(B), container(B).
h(changea(A), t) :- occurs(putin(A, B), t), obj(A), small(A), obj(B), container(B).
h(changeh(A), t) :- occurs(putin(A, B), t), obj(A), small(A), obj(B), container(B).
:- occurs(putin(A, B), t), not h(sameloc(B), t-1), obj(A), small(A), obj(B), container(B).
:- occurs(putin(A, B), t), not h(opened(B), t-1), obj(A), small(A), obj(B), container(B).
:- occurs(putin(A, B), t), not h(hold(A), t-1), obj(A), small(A), obj(B), container(B).

% action takeout(A, B)
h(hold(A), t) :- occurs(takeout(A, B), t), obj(A), small(A), obj(B), container(B).
h(changei(A), t) :- occurs(takeout(A, B), t), obj(A), small(A), obj(B), container(B).
h(changeh(0), t) :- occurs(takeout(A, B), t), obj(A), small(A), obj(B), container(B).
:- occurs(takeout(A, B), t), not h(sameloc(B), t-1), obj(A), small(A), obj(B), container(B).
:- occurs(takeout(A, B), t), not h(inside(A, B), t-1), obj(A), small(A), obj(B), container(B).
:- occurs(takeout(A, B), t), not h(opened(B), t-1), obj(A), small(A), obj(B), container(B).
:- occurs(takeout(A, B), t), not h(hold(0), t-1), obj(A), small(A), obj(B), container(B).


%%assistant predicates
h(inagent(A), t) :- h(hold(A), t), obj(A), small(A).
h(inagent(A), t) :- h(plate(A), t), obj(A), small(A).
h(outagent(A), t) :- not h(inagent(A), t), obj(A), small(A).


%% Static causal laws.
h(sameloc(A), t) :- h(at(0, X), t), h(at(A, X), t), loc(X), obj(A).
h(sameplace(A, B), t) :- h(at(A, X), t), h(at(B, X), t), obj(A), obj(B), loc(X).
:- h(opened(A), t), h(closed(A), t), obj(A), container(A).
h(at(A, X), t) :- h(hold(A), t), h(at(0, X), t), obj(A), small(A), loc(X).
h(at(A, X), t) :- h(plate(A), t), h(at(0, X), t), obj(A), small(A), loc(X).


%% Ineritial
% loc(A, X)
h(at(0, X), t) :- h(at(0, X), t-1), not h(change(0), t), loc(X).
h(at(A, X), t) :- h(at(A, X), t-1), not h(change(A), t), loc(X), obj(A).

h(change(A), t) :- occurs(move(X), t), h(inagent(A), t), obj(A), small(A), loc(X).

% hold(A)
h(hold(A), t) :- h(hold(A), t-1), not h(changeh(A), t), obj(A), small(A).
h(hold(0), t) :- h(hold(0), t-1), not h(changeh(0), t).



% plate(A)
h(plate(A), t) :- h(plate(A), t-1), not h(changep(A), t), obj(A), small(A).
h(plate(0), t) :- h(plate(0), t-1), not h(changep(0), t).

% opened/closed(A)
h(opened(A), t) :- h(opened(A), t-1), not h(closed(A), t), obj(A), container(A).
h(closed(A), t) :- h(closed(A), t-1), not h(opened(A), t), obj(A), container(A).

% inside(A)
h(inside(A, B), t) :- h(inside(A, B), t-1), not h(changei(A), t), obj(A), small(A), obj(B), container(B).


%% Description of constraints.
% cons(task)
 :- task(X, cons(give(human, A))), goal(give(human, A), t).
 :- task(X, cons(puton(A, B))), goal(puton(A, B), t).
 :- task(X, cons(goto(B))), goal(goto(B), t).
 :- task(X, cons(putdown(A))), goal(putdown(A), t).
 :- task(X, cons(pickup(A))), goal(pickup(A), t).
 :- task(X, cons(open(B))), goal(open(B), t).
 :- task(X, cons(close(B))), goal(close(B), t).
 :- task(X, cons(putin(A, B))), goal(putin(A, B), t).
 :- task(X, cons(takeout(A, B))), goal(takeout(A, B), t).

% cons(info)
 :- task(X, cons(at(0, Y))), info(at(0, Y), t).
 :- task(X, cons(at(A, Y))), info(at(A, Y), t).
 :- task(X, cons(inside(A, B))), info(inside(A, B), t).
 :- task(X, cons(on(A, B))), info(on(A, B), t).
 :- task(X, cons(near(A, B))), info(near(A, B), t).
 :- task(X, cons(opened(B))), info(opened(B), t).
 :- task(X, cons(closed(B))), info(closed(B), t).
 :- task(X, cons(plate(A))), info(plate(A), t).
 :- task(X, cons(plate(0))), info(plate(0), t).
 :- task(X, cons(hold(A))), info(hold(A), t).
 :- task(X, cons(hold(0))), info(hold(0), t).

% ncons(info)
ok(X,t) :- task(X, ncons(at(0, Y))), info(at(0, Y), t).
ok(X,t) :- task(X, ncons(at(A, Y))), info(at(A, Y), t).
ok(X,t) :- task(X, ncons(inside(A, B))), info(inside(A, B), t).
ok(X,t) :- task(X, ncons(on(A, B))), info(on(A, B), t).
ok(X,t) :- task(X, ncons(near(A, B))), info(near(A, B), t).
ok(X,t) :- task(X, ncons(opened(B))), info(opened(B), t).
ok(X,t) :- task(X, ncons(closed(B))), info(closed(B), t).
ok(X,t) :- task(X, ncons(plate(A))), info(plate(A), t).
ok(X,t) :- task(X, ncons(plate(0))), info(plate(0), t).
ok(X,t) :- task(X, ncons(hold(A))), info(hold(A), t).
ok(X,t) :- task(X, ncons(hold(0))), info(hold(0), t).
:- task(X, ncons(T)), not ok(X,t).


%% Description of infos
info(at(0, X), t) :- h(at(0, X), t), loc(X).
info(at(A, X), t) :- h(at(A, X), t), obj(A), loc(X).
info(inside(A, B), t) :- h(inside(A, B), t), obj(A), small(A), obj(B), container(B).
info(on(A, B), t) :- h(sameplace(A, B), t), obj(A), obj(B).
info(near(A, B), t) :- h(sameplace(A, B), t), obj(A), obj(B).
info(opened(B), t) :- h(opened(B), t), obj(B), container(B).
info(closed(B), t) :- h(closed(B), t), obj(B), container(B).
info(plate(A), t) :- h(plate(A), t), obj(A), small(A).
info(plate(0), t) :- h(plate(0), t).
info(hold(A), t) :- h(hold(A), t), obj(A), small(A).
info(hold(0), t) :- h(hold(0), t).

%% Description of goals
goal(give(human, A), t) :- h(sameplace(A, B), t), h(outagent(A), t), obj(A), small(A), human(B), obj(B).
goal(puton(A, B), t) :- h(sameplace(A, B), t), h(outagent(A), t), obj(A), small(A), obj(B).
goal(goto(B), t) :- h(sameloc(B), t), obj(B).
goal(putdown(A), t) :- h(outagent(A), t), obj(A), small(A).
goal(pickup(A), t) :- not h(outagent(A), t), obj(A), small(A).
goal(open(B), t) :- h(opened(B), t), obj(B), container(B).
goal(close(B), t) :- h(closed(B), t), obj(B), container(B).
goal(putin(A, B), t) :- h(inside(A, B), t), obj(A), small(A), obj(B), container(B).
goal(takeout(A, B), t) :- not h(inside(A, B), t), obj(A), small(A), obj(B), container(B).


#volatile t.

%%constraints for goals
 :- task(X, give(human, A)), not goal(give(human, A), t).
 :- task(X, puton(A, B)), not goal(puton(A, B), t).
 :- task(X, goto(B)), not goal(goto(B), t).
 :- task(X, putdown(A)), not goal(putdown(A), t).
 :- task(X, pickup(A)), not goal(pickup(A), t).
 :- task(X, open(B)), not goal(open(B), t).
 :- task(X, close(B)), not goal(close(B), t).
 :- task(X, putin(A, B)), not goal(putin(A, B), t).
 :- task(X, takeout(A, B)), not goal(takeout(A, B), t).

#base.
