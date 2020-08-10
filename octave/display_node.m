%octave
% Display a BML node (Id, Data and childs).
function count = display_node(res, cnt, level)

	if exist('level') == 0
   		level = 0;
	end

	count = 0;
	for ii = 1:cnt
		r = res(ii);
    % Cast Id from char array
		if length(r.id) == 4
			id_str =dec2hex(double(typecast(r.id,'uint32')));
		elseif length(r.id) == 8
			id_str =dec2hex(double(typecast(r.id,'uint64')));
		else
			id_str = char(r.id);
		end

    % Print level of child
		for tt = 1:level
			fprintf("-");
		end

		% Print datadata
		if length(r.data)
			fprintf("id:%s off:%d(%d) data:%s(%d) nb_childs:%d :%s\n",id_str, r.offset, r.size, dec2hex(double(char(r.data)))',length(r.data), size(r.childs,2),  char(r.data) );
		else
			fprintf("id:%s off:%d(%d) nb_ext:%d nb_childs:%d\n",id_str, r.offset, r.size, length(r.ext), size(r.childs,2) );
		end

		% recursive display call in order to show childs
		count = count + display_node(r.childs, size(r.childs,2), level+1);
		count = count + 1;
	end
