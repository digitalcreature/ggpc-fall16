$1 ~ /[0-9]+/ && $2 ~ /[0-9]+/ { # match pairs of positive integers
	start = $1 + 0
	end = $2 + 0
	if (start > end) {
		# ensure [end] comes after [start]
		t = start
		start = end
		end = t
	}
	# segments are stored in [segs] as such:
	# segs[start] = end
	for (s in segs) {
		# iterate over existing segments
		e = segs[s]
		# existing:		[s] to [e]
		# new:			[start] to [end]
		if (end >= s - 1 && start <= e + 1) {
			# if existing segment intersects new segment, absorb it
			delete segs[s]
			if (start > s) start = s
			if (end < e) end = e
		}
	}
	# insert new segment
	segs[start] = end
	next
}

# print total length of all segments and start anew
function finish() {
	# make sure segs exists;
	# we dont want to print a bunch of zeroes for multiple blank lines!
	if (length(segs) > 0) {
		len = 0
		for (start in segs) {
			end = segs[start]
			len += (end - start)
		}
		print len
		delete segs
	}
}

# finish counting on blank lines and EOF
{ finish() }
END { finish() }
