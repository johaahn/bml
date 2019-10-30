addpath('./..');

if !exist('port')
 port = 5461;
end

printf("Création d'un serveur sur le port %i\n", port);
fid = bml_open(sprintf("udp://server:%i", port), 's', 4);

printf("Lecture du buffer\n");
[data, cnt] = bml_read(fid, 1024)

printf("Taille des donnees reçues : %i octet(s)\n", prod(size(data(1).data)));

printf("Fermeture du socket\n");
bml_close(fid)

% port++

