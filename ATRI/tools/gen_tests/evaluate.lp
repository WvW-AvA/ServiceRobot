% ASP file for @Home Simulation 2012
% The file is running for clingo or gringo + clasp or iclingo
% vstate.lp and vaction.lp are needed for initial state and the action.
% forcons.lp is also needed to check whether the constraint is satisfied by the next state.
% Written by Jianmin JI. jianmin@ustc.edu.cn
% 2012-10-22


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
action_of(askloc(A)) :- obj(A).
action_of(askloc(0)).
action_of(sense).
% restruction.
:- occurs(A, 1), not action_of(A).


%% Descriptions of actions.
% action move(X)
h(at(0, X), 1) :- occurs(move(X), 1), loc(X).
nh(at(0, Y), 1) :- occurs(move(X), 1), X!=Y, h(at(0, Y), 0), loc(X), loc(Y).
:- occurs(move(X), 1), h(at(0, X), 0), loc(X).

% action pickup(A)
h(hold(A), 1) :- occurs(pickup(A), 1), obj(A), small(A).
nh(hold(0), 1) :- occurs(pickup(A), 1), obj(A), small(A).
:- occurs(pickup(A), 1), not h(hold(0), 0), obj(A), small(A).
:- occurs(pickup(A), 1), h(plate(A), 0), obj(A), small(A).
:- occurs(pickup(A), 1), not h(sameloc(A), 0), obj(A), small(A).

% action putdown(A)
h(hold(0), 1) :- occurs(putdown(A), 1), obj(A), small(A).
nh(hold(A), 1) :- occurs(putdown(A), 1), obj(A), small(A).
:- occurs(putdown(A), 1), not h(hold(A), 0), obj(A), small(A).

% action toplate(A)
h(plate(A), 1) :- occurs(toplate(A), 1), obj(A), small(A).
h(hold(0), 1) :- occurs(toplate(A), 1), obj(A), small(A).
nh(hold(A), 1) :- occurs(toplate(A), 1), obj(A), small(A).
nh(plate(0), 1) :- occurs(toplate(A), 1), obj(A), small(A).
:- occurs(toplate(A), 1), not h(hold(A), 0), obj(A), small(A).
:- occurs(toplate(A), 1), not h(plate(0), 0), obj(A), small(A).

% action fromplate(A)
h(hold(A), 1) :- occurs(fromplate(A), 1), obj(A), small(A).
h(plate(0), 1) :- occurs(fromplate(A), 1), obj(A), small(A).
nh(hold(0), 1) :- occurs(fromplate(A), 1), obj(A), small(A).
nh(plate(A), 1) :- occurs(fromplate(A), 1), obj(A), small(A).
:- occurs(fromplate(A), 1), not h(plate(A), 0), obj(A), small(A).
:- occurs(fromplate(A), 1), not h(hold(0), 0), obj(A), small(A).

% action open(A) 
h(opened(A), 1) :- occurs(open(A), 1), obj(A), container(A).
:- occurs(open(A), 1), not h(hold(0), 0), obj(A), container(A).
:- occurs(open(A), 1), not h(closed(A), 0), obj(A), container(A).
:- occurs(open(A), 1), not h(sameloc(A), 0), obj(A), container(A).

% action close(A)
h(closed(A), 1) :- occurs(close(A), 1), obj(A), container(A).
:- occurs(close(A), 1), not h(hold(0), 0), obj(A), container(A).
:- occurs(close(A), 1), not h(sameloc(A), 0), obj(A), container(A).
:- occurs(close(A), 1), not h(opened(A), 0), obj(A), container(A).

% action putin(A, B) 
h(inside(A, B), 1) :- occurs(putin(A, B), 1), obj(A), small(A), obj(B), container(B).
h(hold(0), 1) :- occurs(putin(A, B), 1), obj(A), small(A), obj(B), container(B).
nh(at(A, X), 1) :- occurs(putin(A, B), 1), h(at(0, X), 0), loc(X), obj(A), small(A), obj(B), container(B).
nh(hold(A), 1) :- occurs(putin(A, B), 1), obj(A), small(A), obj(B), container(B).
:- occurs(putin(A, B), 1), not h(sameloc(B), 0), obj(A), small(A), obj(B), container(B).
:- occurs(putin(A, B), 1), not h(opened(B), 0), obj(A), small(A), obj(B), container(B).
:- occurs(putin(A, B), 1), not h(hold(A), 0), obj(A), small(A), obj(B), container(B).

% action takeout(A, B)
h(hold(A), 1) :- occurs(takeout(A, B), 1), obj(A), small(A), obj(B), container(B).
nh(inside(A, B), 1) :- occurs(takeout(A, B), 1), obj(A), small(A), obj(B), container(B).
nh(hold(0), 1) :- occurs(takeout(A, B), 1), obj(A), small(A), obj(B), container(B).
:- occurs(takeout(A, B), 1), not h(sameloc(B), 0), obj(A), small(A), obj(B), container(B).
:- occurs(takeout(A, B), 1), not h(inside(A, B), 0), obj(A), small(A), obj(B), container(B).
:- occurs(takeout(A, B), 1), not h(opened(B), 0), obj(A), small(A), obj(B), container(B).
:- occurs(takeout(A, B), 1), not h(hold(0), 0), obj(A), small(A), obj(B), container(B).


%% Static causal laws.
h(sameloc(A), 0) :- h(at(0, X), 0), h(at(A, X), 0), loc(X), obj(A).
:- h(opened(A), 0), h(closed(A), 0), obj(A), container(A).
:- h(opened(A), 1), h(closed(A), 1), obj(A), container(A).
h(at(A, X), 0) :- h(hold(A), 0), h(at(0, X), 0), obj(A), small(A), loc(X).
h(at(A, X), 1) :- h(hold(A), 1), h(at(0, X), 1), obj(A), small(A), loc(X).
h(at(A, X), 0) :- h(plate(A), 0), h(at(0, X), 0), obj(A), small(A), loc(X).
h(at(A, X), 1) :- h(plate(A), 1), h(at(0, X), 1), obj(A), small(A), loc(X).
nh(at(A, X), 1) :- h(at(A, Y), 1), X!=Y, obj(A), small(A), loc(X), loc(Y).


%% Ineritial
% loc(A, X)
h(at(0, X), 1) :- h(at(0, X), 0), not nh(at(0, X), 1), loc(X).
h(at(A, X), 1) :- h(at(A, X), 0), not nh(at(A, X), 1), loc(X), obj(A).

% hold(A)
h(hold(A), 1) :- h(hold(A), 0), not nh(hold(A), 1), obj(A), small(A).
h(hold(0), 1) :- h(hold(0), 0), not nh(hold(0), 1).

% plate(A)
h(plate(A), 1) :- h(plate(A), 0), not nh(plate(A), 1), obj(A), small(A).
h(plate(0), 1) :- h(plate(0), 0), not nh(plate(0), 1).

% opened/closed(A)
h(opened(A), 1) :- h(opened(A), 0), not h(closed(A), 1), obj(A), container(A).
h(closed(A), 1) :- h(closed(A), 0), not h(opened(A), 1), obj(A), container(A).

% inside(A)
h(inside(A, B), 1) :- h(inside(A, B), 0), not nh(inside(A, B), 1), obj(A), small(A), obj(B), container(B).


%% ask action askloc(A)
answer(at(A, X), 1) :- occurs(askloc(A), 1), h(at(A, X), 0), obj(A).
answer(at(0, X), 1) :- occurs(askloc(0), 1), h(at(0, X), 0), loc(X).
answer(inside(A, B), 1) :- occurs(askloc(A), 1), h(inside(A, B), 0), obj(A), small(A), obj(B), container(B).

%% sense action sense
sensed(A, 1) :- occurs(sense, 1), h(sameloc(A), 0), obj(A), A!=0.
sensed(A, 1) :- occurs(sense, 1), h(inside(A, B), 0), h(opened(B), 0), h(sameloc(B), 0), obj(A), small(A), obj(B), container(B).

#hide.
#show h(X, 1).
% #show occurs/2.
#show answer/2.
#show sensed/2.
#show false/2.