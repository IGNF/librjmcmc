import os

if __name__ == "__main__" :
	commande = "./RJMCMC_Rectangles_noihm -h"
	print (commande)
	os.system(commande)

	files = os.listdir('.')
	inputs = []
	for myfile in files :
		if myfile.endswith('.tif'):
		 if myfile.startswith('test_'):
			inputs.append(myfile.replace('.tif',''))

	for input in inputs :
		commande = "./RJMCMC_Rectangles_noihm -c test.cfg -i " + input + ".tif -o out_" + input + "_iter"
		print (commande)
		os.system(commande)

