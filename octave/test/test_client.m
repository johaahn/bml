addpath('./..');

ip = "127.0.0.1";
if !exist('port')
 port = 5461;
end

printf("Connexion d'un client à l'adresse %s sur le port %i\n", ip, port);
fid = bml_open(sprintf("udp://%s:%i", ip, port), 's', 4);

sleep(1);

printf("Ecriture du buffer\n");
bml_write(fid, struct('id', 1, 'data', 'toto'));

sleep(1);

printf("Fermeture du socket\n");
bml_close(fid)


% port++

