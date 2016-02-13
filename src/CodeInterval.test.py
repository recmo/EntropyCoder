#!/usr/bin/env python3

def floor64(n):
	return n // 2**64

def ceil64(n):
	if (n % 2**64) > 0:
		return 1 + (n // 2**64)
	else:
		return n // 2**64

def update(ran, sym):
	bB = ran[0]
	rB = ran[1]
	bS = sym[0]
	rS = sym[1]
	# print(bB, rB, bS, rS)
	assert(rB >= 2**63)
	assert(bS + rS + 1 <= 2**64)
	assert(rS >= 2)
	bBp = bB + ceil64(bS * (rB + 1))
	rBp = floor64((bS + rS + 1) * (rB + 1)) - ceil64(bS * (rB + 1)) - 1
	carry = False
	if bBp >= 2**64:
		bBp -= 2**64
		carry = True
	# print(bBp, rBp, carry)
	assert(bBp >= 0)
	assert(bBp < 2**64)
	assert(rBp >= 0)
	assert(rBp < 2**64)
	return [bBp, rBp, carry]

def lower_bound(bS, rB):
	for rS in range(100):
		try:
			update([0, rB + 2**63], [bS, rS])
		except:
			continue
		return rS

L = 0
for bS in range(10):
	for rB in range(10):
		l = lower_bound(bS, rB)
		L = l if l > L else L
		#print(bS, rB, l)

values = dict(
	c0 = 0x0000000000000000,
	c1 = 0x0000000000000001,
	c2 = 0x0000000000000002,
	c3 = 0x0000000000000003,
	c5 = 0x5555555555555555,
	c7 = 0x7FFFFFFFFFFFFFFF,
	c8 = 0x8000000000000000,
	c9 = 0x8000000000000001,
	cA = 0xAAAAAAAAAAAAAAAA,
	cE = 0xFFFFFFFFFFFFFFFE,
	cF = 0xFFFFFFFFFFFFFFFF
)

def intervals(names):
	return dict(zip(names, [[values['c'+n[1]], values['c'+n[2]]] for n in names]))

symbols = intervals([
	                   'i03', 'i05', 'i07', 'i08', 'i09', 'i0A', 'i0E', 'i0F',
	                   'i13', 'i15', 'i17', 'i18', 'i19', 'i1A', 'i1E',
	                   'i23', 'i25', 'i27', 'i28', 'i29', 'i2A',
	                   'i33', 'i35', 'i37', 'i38', 'i39', 'i3A',
	                   'i53', 'i55', 'i57', 'i58', 'i59', 'i5A',
	                   'i73', 'i75', 'i77', 'i78',
	                   'i83', 'i85', 'i87',
	                   'i93', 'i95',
	                   'iA3', 'iA5',
])

normalized = intervals([
	                                        'i08', 'i09', 'i0A', 'i0E', 'i0F',
	                                        'i18', 'i19', 'i1A', 'i1E', 'i1F',
	                                        'i28', 'i29', 'i2A', 'i2E', 'i2F',
	                                        'i38', 'i39', 'i3A', 'i3E', 'i3F',
	                                        'i58', 'i59', 'i5A', 'i5E', 'i5F',
	                                        'i78', 'i79', 'i7A', 'i7E', 'i7F',
	                                        'i88', 'i89', 'i8A', 'i8E', 'i8F',
	                                        'i98', 'i99', 'i9A', 'i9E', 'i9F',
	                                        'iA8', 'iA9', 'iAA', 'iAE', 'iAF',
	                                        'iE8', 'iE9', 'iEA', 'iEE', 'iEF',
	                                        'iF8', 'iF9', 'iFA', 'iFE', 'iFF'
])

ran_idx = 0
for ran in sorted(normalized):
	sym_idx = 0
	for sym in sorted(symbols):
		upd = update(normalized[ran], symbols[sym])		
		result = "\t{{0x{:0>16X}UL, 0x{:0>16X}UL, {: >2}, {: >2}, {}}},"\
		.format(upd[0], upd[1], ran_idx, sym_idx, "true " if upd[2] else "false")
		print(result)
		sym_idx += 1
	ran_idx += 1
