import os
import sys

def fileName(taskPath, l):
	res = "../plots/"
	
	res = res + taskPath.split("/")[-1][:-4] + "-" + ("-").join(l) + ".png"
	
	return res
	
def printUsage():
	print "-----------------------------------------------------"
	print "Uso:"
	print "python run-and-save.py task-path num-cores costo-CS costo-MI sched-name [sched-params]"
	print "-----------------------------------------------------"

def main(argc, argv):
	if argc < 6:
		printUsage()
		return
	
	taskPath = argv[1]
	numCores = argv[2]
	costoCS = argv[3]
	costoMI = argv[4]
	schedName = argv[5]
	if (argv > 6): schedParams = argv[6:]
	else: schedParams = []
	command = "./simusched " + taskPath + " " + numCores + " " + costoCS + " " + costoMI + " " + schedName + " " + (" ").join(schedParams) + " | python graphsched.py > " + fileName(taskPath, argv[2:])
	print "Executing:", command
	os.system(command)
	
if __name__ == '__main__':
	main(len(sys.argv), sys.argv)
