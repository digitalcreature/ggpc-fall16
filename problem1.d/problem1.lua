-- DEPRECATED
-- half-finished lua implementation; i'll finish it one day

segs = {}

function segs:printtotal()
	local seg = segs.next;
	local total = 0
	while seg do
		total = total + #seg
		seg = seg.next
	end
	print(total)
end

local segment_mt = {
	__len = function(self)
		return self.b - self.a
	end
}

function segment(a, b)
	return setmetatable({
		a = math.min(a, b),
		b = math.max(a, b),
	}, segment_mt)
end

function segs:add(new)
	if #new == 0 then return end
	local seg = segs
	local a, b
	while seg.next do
		if new.a <= seg.b then
			if new.b >= seg.a then return end
			if not a then
				if seg.a <= new.b then
					a = seg
					if seg.a < new.a then
						new.a = seg.a
					end
				else
					new.next = seg
					new.prev = seg.prev
					seg.prev.next = new
					seg.prev = new
					return
				end
			end
			if a then
				if new.b >= seg.a then
					b = seg
					if new.b <= seg.b then
						new.b = seg.b
						break
					end
				else
					break
				end
			end
		end
		seg = seg.next
	end
	if a then
		new.prev = a.prev
		a.prev.next = new
		new.next = b
		b.next.prev = new
	else
		new.prev = seg
		seg.next = new
	end
end

for line in io.lines() do
	local running
	if line:match("%s*") then

end
