all:
	cd src && make && mv ps3event_clientd ..

clean:
	cd src && make clean

mrpropre:
	rm ps3event_clientd && cd src && make clean
