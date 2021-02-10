######################################################
# COMP7500 - Project 2 - Pipe-based Wordcount Tool
# Name: Hannah Reinbolt
# Date: 2/9/2021
######################################################

# compile pwordcount executable
pwordcount: pwordcount.c helper.c
	@echo "Starting to compile pwordcount..."
	gcc -o pwordcount pwordcount.c helper.c
	@echo "Finished!"

# delete old pwordcount executable
clean:
	@echo "Removing pwordcount executable..."
	rm -f pwordcount
	@echo "Finished!"
