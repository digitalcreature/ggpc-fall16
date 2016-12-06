$1 ~ /[0-9]+/ && $2 ~ /[0-9]+/ {
	start = $1 + 0
	end = $2 + 0
	if (start > end) {
		t = start
		start = end
		end = t
	}
	for (s in segs) {
		e = segs[s]
		if (end >= s - 1 && start <= e + 1) {
			delete segs[s]
			if (start > s) start = s
			if (end < e) end = e
		}
	}
	segs[start] = end
	next
}

function finish() {
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

{ finish() }
END { finish() }
