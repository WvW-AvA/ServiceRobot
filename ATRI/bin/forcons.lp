% ASP file for @Home Simulation 2012
% The file is running for clingo or gringo + clasp or iclingo
% forcons.lp plus with vcons.lp, vstate.lp to decide whether a constraint is satisfied so far.
% Written by Jianmin JI. jianmin@ustc.edu.cn
% 2012-10-23

%% Description of goals
goal(give(human, A), 1) :- hold(sameplace(A, B), 1), hold(outagent(A), 1), obj(A), small(A), human(B), obj(B).
goal(puton(A, B), 1) :- hold(sameplace(A, B), 1), hold(outagent(A), 1), obj(A), small(A), obj(B).
goal(goto(B), 1) :- hold(sameloc(B), 1), obj(B).
goal(putdown(A), 1) :- hold(outagent(A), 1), obj(A), small(A).
goal(pickup(A), 1) :- not hold(outagent(A), 1), obj(A), small(A).
goal(open(B), 1) :- h(opened(B), 1), obj(B), container(B).
goal(close(B), 1) :- h(closed(B), 1), obj(B), container(B).
goal(putin(A, B), 1) :- h(inside(A, B), 1), obj(A), small(A), obj(B), container(B).
goal(takeout(A, B), 1) :- not h(inside(A, B), 1), obj(A), small(A), obj(B), container(B).


%% Static causal laws
hold(sameplace(A, B), 1) :- h(at(A, X), 1), h(at(B, X), 1), obj(A), obj(B), loc(X).
hold(sameloc(B), 1) :- h(at(B, X), 1), h(at(0, X), 1), obj(B), loc(X).
hold(outagent(A), 1) :- not h(hold(A), 1), not h(plate(A), 1), obj(A), small(A).
%hold(outagent(A), 1) :- not h(hold(A), 1), obj(A), small(A).
%hold(outagent(A), 1) :- not h(plate(A), 1), obj(A), small(A).


%% Description of infos
info(at(0, X), 1) :- h(at(0, X), 1), loc(X).
info(at(A, X), 1) :- h(at(A, X), 1), obj(A), loc(X).
info(inside(A, B), 1) :- h(inside(A, B), 1), obj(A), small(A), obj(B), container(B).
info(on(A, B), 1) :- hold(sameplace(A, B), 1), obj(A), obj(B).
info(near(A, B), 1) :- hold(sameplace(A, B), 1), obj(A), obj(B).
info(opened(B), 1) :- h(opened(B), 1), obj(B), container(B).
info(closed(B), 1) :- h(closed(B), 1), obj(B), container(B).
info(plate(A), 1) :- h(plate(A), 1), obj(A), small(A).
info(plate(0), 1) :- h(plate(0), 1).
info(hold(A), 1) :- h(hold(A), 1), obj(A), small(A).
info(hold(0), 1) :- h(hold(0), 1).


%% Description of constraints.
% cons(task)
false(X, cons) :- task(X, cons(give(human, A))), goal(give(human, A), 1).
false(X, cons) :- task(X, cons(puton(A, B))), goal(puton(A, B), 1).
false(X, cons) :- task(X, cons(goto(B))), goal(goto(B), 1).
false(X, cons) :- task(X, cons(putdown(A))), goal(putdown(A), 1).
false(X, cons) :- task(X, cons(pickup(A))), goal(pickup(A), 1).
false(X, cons) :- task(X, cons(open(B))), goal(open(B), 1).
false(X, cons) :- task(X, cons(close(B))), goal(close(B), 1).
false(X, cons) :- task(X, cons(putin(A, B))), goal(putin(A, B), 1).
false(X, cons) :- task(X, cons(takeout(A, B))), goal(takeout(A, B), 1).

% cons(info)
false(X, cons) :- task(X, cons(at(0, Y))), info(at(0, Y), 1).
false(X, cons) :- task(X, cons(at(A, Y))), info(at(A, Y), 1).
false(X, cons) :- task(X, cons(inside(A, B))), info(inside(A, B), 1).
false(X, cons) :- task(X, cons(on(A, B))), info(on(A, B), 1).
false(X, cons) :- task(X, cons(near(A, B))), info(near(A, B), 1).
false(X, cons) :- task(X, cons(opened(B))), info(opened(B), 1).
false(X, cons) :- task(X, cons(closed(B))), info(closed(B), 1).
false(X, cons) :- task(X, cons(plate(A))), info(plate(A), 1).
false(X, cons) :- task(X, cons(plate(0))), info(plate(0), 1).
false(X, cons) :- task(X, cons(hold(A))), info(hold(A), 1).
false(X, cons) :- task(X, cons(hold(0))), info(hold(0), 1).

% ncons(info)
false(X, ncons) :- task(X, ncons(at(0, Y))), not info(at(0, Y), 1).
false(X, ncons) :- task(X, ncons(at(A, Y))), not info(at(A, Y), 1).
false(X, ncons) :- task(X, ncons(inside(A, B))), not info(inside(A, B), 1).
false(X, ncons) :- task(X, ncons(on(A, B))), not info(on(A, B), 1).
false(X, ncons) :- task(X, ncons(near(A, B))), not info(near(A, B), 1).
false(X, ncons) :- task(X, ncons(opened(B))), not info(opened(B), 1).
false(X, ncons) :- task(X, ncons(closed(B))), not info(closed(B), 1).
false(X, ncons) :- task(X, ncons(plate(A))), not info(plate(A), 1).
false(X, ncons) :- task(X, ncons(plate(0))), not info(plate(0), 1).
false(X, ncons) :- task(X, ncons(hold(A))), not info(hold(A), 1).
false(X, ncons) :- task(X, ncons(hold(0))), not info(hold(0), 1).
