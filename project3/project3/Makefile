################################################################
# COMP7500 - Project 3 - A Pthread-based Batch Scheduling Tool
# Name: Hannah Reinbolt
# Date: 3/14/2021
################################################################


# compile aubatch executable
aubatch: aubatch.c commandline.o schedule.o
	@echo "Starting to compile aubatch..."
	gcc -o aubatch aubatch.c commandline.o schedule.o -lpthread -Wall
	@echo "Finished!"

# compile commandline executable
commandline.o: commandline.c
	@echo "Starting to compile commandline..."
	gcc -c commandline.c -lpthread -Wall
	@echo "Finished!"

# compile schedule executable
schedule.o: schedule.c
	@echo "Strated to compile schedule..."
	gcc -c schedule.c -lpthread -Wall
	@echo "Finished!"


# delete old aubatch executable
clean:
	@echo "Removing pwordcount executable..."
	rm -f aubatch
	@echo "Finished!"
