from testing_python import checker

mycheck = checker.checker()
mycheck.project = "libMPP"
# Destinataires du mail
mycheck.recipients = ['didier.boldo@ign.fr','olivier.tournaire@ign.fr']
mycheck.check()
