compila: clean
	g++ -O2 main.cpp `allegro-config --libs` -o AliceInClicheland
run: compila
	./AliceInClicheland
clean:
	rm -rf AliceInClicheland
