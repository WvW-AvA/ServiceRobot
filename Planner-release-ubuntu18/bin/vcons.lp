task(1, cons(opened(X))) :- refrigerator(X).
task(2, cons(puton(X, Y))) :- book(X), green(X), closet(Y).
task(3, ncons(on(X, Y))) :- bottle(X), green(X), desk(Y).
