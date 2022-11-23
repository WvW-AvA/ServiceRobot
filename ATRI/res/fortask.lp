% ASP file for @Home Simulation 2012
% The file is running for clingo or gringo + clasp or iclingo
% fortask.lp plus with vtask.lp, vstate.lp to decide whether tasks are solved.
% plus show.lp only for scores.
% Written by Jianmin JI. jianmin@ustc.edu.cn
% 2012-10-23

%% Description of goals
goal(give(human, A), 0) :- h(sameplace(A, B), 0), h(outagent(A), 0), obj(A), small(A), human(B), obj(B).
goal(puton(A, B), 0) :- h(sameplace(A, B), 0), h(outagent(A), 0), obj(A), small(A), obj(B).
goal(goto(B), 0) :- h(sameloc(B), 0), obj(B).
goal(putdown(A), 0) :- h(outagent(A), 0), obj(A), small(A).
goal(pickup(A), 0) :- not h(outagent(A), 0), obj(A), small(A).
goal(open(B), 0) :- h(opened(B), 0), obj(B), container(B).
goal(close(B), 0) :- h(closed(B), 0), obj(B), container(B).
goal(putin(A, B), 0) :- h(inside(A, B), 0), obj(A), small(A), obj(B), container(B).
goal(takeout(A, B), 0) :- not h(inside(A, B), 0), obj(A), small(A), obj(B), container(B).


%% Static causal laws
h(sameplace(A, B), 0) :- h(at(A, X), 0), h(at(B, X), 0), obj(A), obj(B), loc(X).
h(sameloc(B), 0) :- h(at(B, X), 0), h(at(0, X), 0), obj(B), loc(X).
h(outagent(A), 0) :- not h(hold(A), 0), not h(plate(A), 0), obj(A), small(A).
%h(outagent(A), 0) :- not h(hold(A), 0), obj(A), small(A).
%h(outagent(A), 0) :- not h(plate(A), 0), obj(A), small(A).


%% Restriction of tasks
true(X, goal) :- task(X, give(human, A)), goal(give(human, A), 0).
true(X, goal) :- task(X, puton(A, B)), goal(puton(A, B), 0).
true(X, goal) :- task(X, goto(B)), goal(goto(B), 0).
true(X, goal) :- task(X, putdown(A)), goal(putdown(A), 0).
true(X, goal) :- task(X, pickup(A)), goal(pickup(A), 0).
true(X, goal) :- task(X, open(B)), goal(open(B), 0).
true(X, goal) :- task(X, close(B)), goal(close(B), 0).
true(X, goal) :- task(X, putin(A, B)), goal(putin(A, B), 0).
true(X, goal) :- task(X, takeout(A, B)), goal(takeout(A, B), 0).


%% Scores
true(X, cons) :- not false(X, cons), task(X, cons(Y)).
true(X, ncons) :- not false(X, ncons), task(X, ncons(Y)).
value(X, 40) :- true(X, goal).
true(goal) :- true(X, goal).
value(X, 20) :- true(X, cons), true(goal).
value(X, 20) :- true(X, ncons), true(goal).