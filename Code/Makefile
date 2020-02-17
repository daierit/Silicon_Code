CXX = g++ -std=c++11 -O3 -Wall
LDP = $(shell root-config --ld)
LIBP = $(shell root-config --libs)

all :
	make HGcal128Ch_IV
	make FullWafer_distrubution
	make FullWafer_plot

HGcal128Ch_IV.o : HGcal128Ch_IV.C
	$(CXX) -c -o $@ -g $< -MMD
-include HGcal128Ch_IV.d

HGcal128Ch_IV : HGcal128Ch_IV.o
	$(LDP) -o $@ $+ $(LIBP)

FullWafer_distrubution.o : FullWafer_distrubution.C
	$(CXX) -c -o $@ -g $< -MMD
-include FullWafer_distrubution.d

FullWafer_distrubution : FullWafer_distrubution.o
	$(LDP) -o $@ $+ $(LIBP)

FullWafer_plot.o : FullWafer_plot.C
	$(CXX) -c -o $@ -g $< -MMD
-include FullWafer_plot.d

FullWafer_plot : FullWafer_plot.o
	$(LDP) -o $@ $+ $(LIBP)
