% Compile BML extractor
filename = '/tmp/test.bml'
fid = bml_open(filename, 'r');
id = ['1'];
count = 0;
while size(id)
	[id, data, level] = bml_read(fid);
	count++;
	
	if size(id,1)
		if length(id) == 4
			id_str =dec2hex(double(typecast(id,'uint32')));
		elseif length(id) == 8
			id_str =dec2hex(double(typecast(id,'uint64')));
		else
			id_str = char(id);
		end
		data
		if length(data) 
		printf("*** id:%s lvl:%d data:%s %s\n",id_str,level, dec2hex(double(char(data)))',  char(data))
		else
		printf("*** id:%s lvl:%d\n",id_str,level);
		end
	end
end
count
