#top-level makefile
#by Yifan Xu, 03/19/2017

all: help
	cd src; make
	
## For Main App
runs:
	./bin/Server 2009
runc:
	./bin/Client crystalcove.eecs.uci.edu 2009
	
help:
	@echo "---------------------------------------------------------------------------------------------------"
	@echo "To run this application, first start the server in one terminal,"
	@echo "then start the client in another."
	@echo
	@echo "*How to Use? Just type:"
	@echo "make runs: Run Server with Default Settings"
	@echo "make runc: Run Client with Default Settings"
	@echo "---------------------------------------------------------------------------------------------------"

## For Test
test: testhelp
	cd src; make test
testsui:
	./bin/TestServerUI
testcui:
	./bin/TestClientUI
runsd:
	./bin/ServerD 2009
runcd:
	./bin/ClientD crystalcove.eecs.uci.edu 2009
runst:
	./bin/TestModeServer 2009
runct:
	./bin/TestModeClient crystalcove.eecs.uci.edu 2009
testpf: 
	./bin/TestPathfind
test-comm:
	./bin/AutoTest crystalcove.eecs.uci.edu 2009

testhelp:
	@echo "---------------------------------------------------------------------------------------------------"
	@echo "Test Manual"
	@echo ""
	@echo "make runcd:	Run Client under Debug Mode, display mid value"
	@echo "make runsd:	Run Server under Debug Mode, display mid value"
	@echo "make runct:	Run Tese Mode Client in order to test Server Terminal"
	@echo "make runst:	Run Tese Mode Server in order to test Server Terminal"
	@echo "make testsui:	Run Server demo (only UI), no Socket or any other functions"
	@echo "make testcui:	Run Client demo (only UI), no Socket or any other functions"
	@echo "make testpf:	Run Pathfind Test Module, test core system function(Find shortest path from start to the destination)"	
	@echo "make test-comm: Run auto-test module, test functions of Server Core System and Socket communication"
	@echo "---------------------------------------------------------------------------------------------------"

## For Support
clean:
	rm -rf ./src/*.o
	rm -rf ./bin/Client
	rm -rf ./bin/ClientD
	rm -rf ./bin/Server
	rm -rf ./bin/ServerD
	rm -rf ./bin/TestModeServer
	rm -rf ./bin/TestModeClient
	rm -rf ./bin/TestClientUI
	rm -rf ./bin/TestServerUI
	rm -rf ./bin/AutoTest
	rm -rf ./bin/TestPathfind

cleano:
	rm -rf ./src/*.o

tar:
	gtar cvzf Taxi_V1.0.tar.gz bin --exclude 'CVS' doc --exclude 'CVS' --exclude 'Taxi_SoftwareSpec.pdf' README INSTALL COPYRIGHT Makefile
tarsrc:
	gtar cvzf Taxi_V1.0_src.tar.gz bin --exclude 'CVS' doc --exclude 'CVS' src --exclude 'CVS' --exclude '*.tar.gz' README Makefile INSTALL COPYRIGHT
