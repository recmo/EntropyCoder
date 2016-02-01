# Entropy Coding

\newcommand{\brace}[3]{\left#1{#3}\right#2}
\newcommand{\floor}[1]{\brace⌊⌋{#1}}
\newcommand{\ceil}[1]{\brace⌈⌉{#1}}

In these lab notes I derive an arithmetic coder that is optimally efficient using 64 bit registers.


## Range multiplication

Given two sub-intervals of $\brace[){0, 1}$:

$$
\begin{aligned}
B &= \brace[){b₀, b₁} \\
S &= \brace[){s₀, s₁}
\end{aligned}
$$

and we want to compute the product of $S$ into $B$:

$$
S · B = \brace[){bₒ + s₀ (b₁ - b₀), bₒ + s₁(b₁ - b₀)}
$$

## 64 bit intervals

We want to represent non-empty subintervals of $\brace[){0,1}$ with 64 bit integers, $𝔹⁶⁴$.

\newcommand{\b}{\mathtt{b}}
\newcommand{\r}{\mathtt{r}}
\newcommand{\GF}[1]{\mathrm{GF}\brace(){#1}}

Take $\brace(){\b,\r} ∊ \brace(){𝔹⁶⁴}²$ with the following map:

$$
\brace(){\b,\r} ↦ \brace[){\frac{\b}{2⁶⁴}, \frac{\b + \r + 1}{2⁶⁴}}
$$

Some examples of this map:

$$
\begin{aligned}
\brace(){0,0} &↦ \brace[){0, 2⁻⁶⁴} \\
\brace(){0,2⁶⁴-1} &↦ \brace[){0, 1} \\
\brace(){2⁶⁴-1, 0} &↦ \brace[){1 - 2⁻⁶⁴, 1} \\
\brace(){2⁶⁴-1, 2⁶⁴-1} &↦ \brace[){1 - 2⁻⁶⁴, 2 - 2⁻⁶⁴} \\
\end{aligned}
$$

Where the last example is invalid. For validity we require

$$
\b + \r + 1 ≤ 2⁶⁴
$$

A reverse map can be created as:

$$
\brace[){l, h} ↦ \brace(){\ceil{l · 2⁶⁴}, \floor{h · 2⁶⁴} - \ceil{l · 2⁶⁴} - 1}
$$

In order for $\b$ to be in $𝔹⁶⁴$ we require $0 ≤ \ceil{l · 2⁶⁴} < 2⁶⁴$. We already have $0 ≤ l < 1$. To satisfy the upper bound we additionally require:

$$
l ≤ 1 - 2⁻⁶⁴
$$

In order for $\r$ to be in $𝔹⁶⁴$ we require $0 ≤ \floor{h · 2⁶⁴} - \ceil{l · 2⁶⁴} - 1 < 2⁶⁴$. The upper bound is already satisfied. To satisfy the lower bound we require:

$$
\begin{aligned}
0 &≤ \floor{h · 2⁶⁴} - \ceil{l · 2⁶⁴} - 1 \\
&< \brace(){h - l} · 2⁶⁴ - 1 - 1 \\
l + 2⁻⁶³ &< h
\end{aligned}
$$

Since $h ≤ 1$ this also implies the stronger bound $l < 1 - 2⁻⁶³$ on $l$. For the reverse map to work we need $l + 2⁻⁶³ < h ≤ 1$.

Going full circle results in:

$$
\brace[){l, h} ↦ \brace(){\b,\r} ↦ \brace[){\frac{\ceil{l · 2⁶⁴}}{2⁶⁴}, \frac{\floor{h · 2⁶⁴}}{2⁶⁴}}
$$

It can be easily shown that the later interval is always a subinterval of the former. It is also easily seen that this operation is idempotent.

## 64 bit interval multiply

$$
\begin{aligned}
\brace(){\b_B,\r_B} &↦ \brace[){\frac{\b_B}{2⁶⁴}, \frac{\b_B + \r_B + 1}{2⁶⁴}} \\
\brace(){\b_S,\r_S} &↦ \brace[){\frac{\b_S}{2⁶⁴}, \frac{\b_S + \r_S + 1}{2⁶⁴}}
\end{aligned}
$$

$$
\brace[){
  \frac{\b_B}{2⁶⁴} + \frac{\b_S}{2⁶⁴} \brace(){\frac{\b_B + \r_B + 1}{2⁶⁴} - \frac{\b_B}{2⁶⁴}},
  \frac{\b_B}{2⁶⁴} + \frac{\b_S + \r_S + 1}{2⁶⁴} \brace(){\frac{\b_B + \r_B + 1}{2⁶⁴} - \frac{\b_B}{2⁶⁴}}
}
$$

$$
\brace[){
  \frac{\b_B}{2⁶⁴} + \frac{\b_S\brace(){\r_B + 1}}{2¹²⁸},
  \frac{\b_B}{2⁶⁴} + \frac{\brace(){\b_S + \r_S + 1}\brace(){\r_B + 1}}{2¹²⁸}
}
$$

The first bound in $l + 2⁻⁶³ < h ≤ 1$ gives:

$$
\begin{aligned}
\frac{\b_B}{2⁶⁴} + \frac{\b_S\brace(){\r_B + 1}}{2¹²⁸} + 2⁻⁶³ &< 
\frac{\b_B}{2⁶⁴} + \frac{\brace(){\b_S + \r_S + 1}\brace(){\r_B + 1}}{2¹²⁸} \\
\b_S\brace(){\r_B + 1} + 2⁶⁴ &< 
\brace(){\b_S + \r_S + 1}\brace(){\r_B + 1}
\end{aligned}
$$


The second bound in $l + 2⁻⁶³ < h ≤ 1$ gives:

$$
\begin{aligned}
\frac{\b_B}{2⁶⁴} + \frac{\brace(){\b_S + \r_S + 1}\brace(){\r_B + 1}}{2¹²⁸} & ≤ 1 \\
\b_B · 2⁶⁴ + \brace(){\b_S + \r_S + 1}\brace(){\r_B + 1} & ≤ 2¹²⁸
\end{aligned}
$$


Mapping this back to $\brace(){𝔹⁶⁴}²$ using the reverse map:

$$
\begin{aligned}
\b_B' &= \ceil{l · 2⁶⁴} \\
 &= \ceil{\brace(){ \frac{\b_B}{2⁶⁴} + \frac{\b_S\brace(){\r_B + 1}}{2¹²⁸} }·2⁶⁴} \\
 &= \b_B + \ceil{\frac{\b_S\brace(){\r_B + 1}}{2⁶⁴} } \\
\end{aligned}
$$

Since we have $\b_S < 2⁶⁴$ and $\r_B + 1 ≤ 2⁶⁴$ we can implement this using a 64 bit multiply with 128 bit result, and an 128 bit and 64 bit add as $\b_S · \r_B + \b_S$, or in code:

```cpp
uint64 h, l;
std::tie(h, l) = mul128(b_S, r_B);
std::tie(h, l) = add128(h, l, b_S);
const uint64 t = h + (l > 0 ? 1 : 0);
b_B += t;
```
Where the final term implements the ceiling operator.

$$
\begin{aligned}
\r_B' &= \floor{h · 2⁶⁴} - \ceil{l · 2⁶⁴} - 1 \\
&= \floor{\brace(){\frac{\b_B}{2⁶⁴} + \frac{\brace(){\b_S + \r_S + 1}\brace(){\r_B + 1}}{2¹²⁸}}· 2⁶⁴} - \b_B + \ceil{\frac{\b_S\brace(){\r_B + 1}}{2⁶⁴}} - 1 \\
&= \floor{\frac{\brace(){\b_S + \r_S + 1}\brace(){\r_B + 1}}{2⁶⁴}} - \ceil{\frac{\b_S\brace(){\r_B + 1}}{2⁶⁴}} - 1 \\
\end{aligned}
$$

To obtain a valid result we need $0 ≤ \r_B' < 2⁶⁴$. The lower bound goes like:

$$
\begin{aligned}
0 &≤ \floor{\frac{\brace(){\b_S + \r_S + 1}\brace(){\r_B + 1}}{2⁶⁴}} - \ceil{\frac{\b_S\brace(){\r_B + 1}}{2⁶⁴}} - 1 \\
0 &≤ \frac{\brace(){\b_S + \r_S + 1}\brace(){\r_B + 1}}{2⁶⁴} - \frac{\b_S\brace(){\r_B + 1}}{2⁶⁴} - 1 \\
1 &< \frac{\brace(){\r_S + 1}\brace(){\r_B + 1}}{2⁶⁴} \\
2⁶⁴ &≤ \brace(){\r_S + 1}\brace(){\r_B + 1} \\
\r_S &≥ \frac{2⁶⁴}{\r_B + 1} - 1 \\
\r_S &≥ 1
\end{aligned}
$$

Where in the last step I have used $\r_B ≥ 2⁶³$, which will be enforced later, in normalization. The upper bound $\r_B' < 2⁶⁴$ goes like:

$$
\begin{aligned}
2⁶⁴ &> \floor{\frac{\brace(){\b_S + \r_S + 1}\brace(){\r_B + 1}}{2⁶⁴}} - \ceil{\frac{\b_S\brace(){\r_B + 1}}{2⁶⁴}} - 1 \\
2⁶⁴ &> \frac{\brace(){\b_S + \r_S + 1}\brace(){\r_B + 1}}{2⁶⁴} - \ceil{\frac{\b_S\brace(){\r_B + 1}}{2⁶⁴}} - 1 \\
\end{aligned}
$$


The second term we already have as `t`. For the first term we need another tricky multiply. In this case we have $\b_S + \r_S + 1 ≤ 2⁶⁴$ and $\r_B + 1 ≤ 2⁶⁴$, so the intermediate result can actually be $2⁶⁴$. The $-1$ will make sure the final result will be in $𝔹⁶⁴$. We must be careful, but modular arithmetic will handle the overflow fine. Let's rewrite the multiplication in values $<2⁶⁴$:

$$
\brace(){\b_S + \r_S}·\r_B + \brace(){\b_S + \r_S} + \r_B + 1
$$

Now we can calculate the final interval:

```cpp
std::tie(h, l) = mul128(b_S + r_S, r_B);
std::tie(h, l) = add128(h, l, b_S + r_S);
std::tie(h, l) = add128(h, l, r_B);
std::tie(h, l) = add128(h, l, 1);
r_B = h - t - 1;
```

## Normalization

Given an interval $\brace[){l,h}$ we want to extract the prefix bits that won't
change anymore. To determine which bits won't change it is useful to look at $h-l$:

$$
\begin{aligned}
l &= \mathtt{0.0001101101000111111110110010001001}… \\
h &= \mathtt{0.0001101101001000000011000010000111}… \\
h - l &= \mathtt{0.}
\underbrace{\mathtt{000000000000}}_{\mathrm{settled}}
\underbrace{\mathtt{0000000}}_{\mathrm{outstanding}}
\underbrace{\mathtt{100001111111110}…}_{\mathrm{active}}
\end{aligned}
$$

The settled bits can be written directly to the output. The outstanding bits can still change because of a carry, but are otherwise settled. To normalize this interval we output the first 12 bits, note that there are 7 bits outstanding, and rescale the interval by $2¹²⁺⁷$. There is one edge case we can consider:

$$
\begin{aligned}
h - l &= \mathtt{0.0000000000000000000100000000000000}… \\
&= \mathtt{0.00000000000000000000}
   \underbrace{\mathtt{11111111111111}…}_{\mathrm{active}}
\end{aligned}
$$

Writing the number in two different but equal ways can result in a different number of leading zeros. We take the one that results in the largest number of leading zeros. So in general we want to rescale by $2ⁿ$ with $n$ given by:

$$
n = \floor{-\log₂ \brace(){h - l}} - 1
$$

The interval is normalized if $n=0$ and thus iff $½ < h - l$.

$$
\begin{aligned}
l' = l·2ⁿ &= \mathtt{1101101000111111.110110010001001}… \\
h' = h·2ⁿ &= \mathtt{1101101001000000.011000010000111}… \\
\end{aligned}
$$

Here we can be in one of two situations, either the integral parts are the same, or $h$'s is one larger that $l$'s.
In this case $h$'s is in fact one larger. We can subtract the integral part of $l$:

$$
\begin{aligned}
l'' = l'-\floor{l'} &= \mathtt{0.110110010001001}… \\
h'' = h'-\floor{l'} &= \mathtt{1.011000010000111}… \\
\end{aligned}
$$

We have now scaled our $\brace[){l,h}$ interval to a subinterval of $\brace[){0,2}$, but we also have

$$
0 ≤ l < 1
$$

$$
½ < h-l ≤ 1
$$

In summary, an interval is normalized when it is a subinterval of $\brace[){0,2}$ and the above inequalities hold.

After normalization we end up in one of two cases:

* $\brace[){h,l} ⊆ \brace[){0,1}$
* $\brace[){h,l} ⊈ \brace[){0,1}$

Our situation is as above, there are no bits outstanding.
This is when we have a carry outstanding and $h > 1$.
After further narrowing of the interval we will eventually end up in the first case and flush the carry buffer, or we will end up in a third case:

* $\brace[){h,l} ⊆ \brace[){1,2}$

In this third case we should add the carry and subtract $1$ from both $h$ and $l$. We are then effectively back in the first case.

## 64 bit normalization

$$
\brace[){\frac{\b}{2⁶⁴}, \frac{\b + \r + 1}{2⁶⁴}}
$$

For this to be normalized in $\brace[){0, 2}$ we must have

And the normalization condition $½ < h-l ≤ 1$ reduces to $2⁶³ ≤ \r < 2⁶⁴$. This essentially means that $\r ∊ 𝔹⁶⁴$ and $\r$ must have the most significant bit set.

$$
\begin{aligned}
n &= \floor{-\log₂ \brace(){\frac{\r + 1}{2⁶⁴}}} - 1 \\
&= 63 - \ceil{\log₂ \brace(){\r + 1}} \\
\end{aligned}
$$

The ceiling and the $+1$ cancel, except when $\r=0$. What remains is essentially a count leading zeroes operation:

```cpp
const uint n = r == 0 ? 63 : count_leading_zeros(r);
```

$$
\begin{aligned}
l'' &= l·2ⁿ - \floor{l·2ⁿ} = \brace[]{\frac{\b}{2⁶⁴⁻ⁿ}}₁  \\
h'' &= h·2ⁿ - \floor{l·2ⁿ} = \frac{\b + \r + 1}{2⁶⁴⁻ⁿ} - \floor{\frac{\b}{2⁶⁴⁻ⁿ}}\\
\end{aligned}
$$

Where $\brace[]{x}_1 = x - \floor{x}$ is the fractional part operator.

$$
\begin{aligned}
\b'' &= \ceil{l'' · 2⁶⁴} \\
&= \ceil{\brace[]{\frac{\b}{2⁶⁴⁻ⁿ}}₁ · 2⁶⁴} \\
&= \ceil{\brace[]{\frac{\b}{2⁶⁴⁻ⁿ} · 2⁶⁴}_{2⁶⁴}} \\
&= \brace[]{\b · 2ⁿ}_{2⁶⁴}
\end{aligned}
$$

Where $\brace[]{x}_{2⁶⁴} = x \;\mathrm{mod}\; 2⁶⁴$ is the modular operator. Since $\b · 2ⁿ$ is strictly integer, the ceiling has no effect and the operation reduces to a simple right shift:

```cpp
b <<= n;
```

$$
\begin{aligned}
\r'' &= \floor{h'' · 2⁶⁴} - \ceil{l'' · 2⁶⁴} - 1 \\
&= \floor{h'' · 2⁶⁴} - \b'' - 1 \\
&= \floor{\brace(){\frac{\b + \r + 1}{2⁶⁴⁻ⁿ} - \floor{\frac{\b}{2⁶⁴⁻ⁿ}}} · 2⁶⁴} - \b'' - 1 \\
&= \floor{\brace(){\b + \r + 1}·2ⁿ - \floor{\frac{\b}{2⁶⁴⁻ⁿ}}· 2⁶⁴ } - \b'' - 1 \\
&= \floor{\brace(){\frac{\b·2ⁿ}{2⁶⁴} - \floor{\frac{\b·2ⁿ}{2⁶⁴}}}· 2⁶⁴ + \brace(){\r + 1}·2ⁿ} - \b'' - 1 \\
&= \floor{\brace[]{\b·2ⁿ}_{2⁶⁴} + \brace(){\r + 1}·2ⁿ} - \b'' - 1 \\
&= \floor{\b'' + \brace(){\r + 1}·2ⁿ} - \b'' - 1 \\
&= \r·2ⁿ + 2ⁿ - 1 \\
\end{aligned}
$$

This is essentially shifting $\r$ to the right $n$ places while shifting in ones. We don't need to worry about overflow here because $\r$ is strictly less than $2⁶⁴⁻ⁿ$.

```cpp
r <<= n;
r += (1UL << n) - 1;
```

## Minimum probability

After updating, the smallest $h - l$ is given by $\r = 2⁶³$ and is $\frac 12 + 2⁻⁶⁴$.

## Reverse


## STOKE Optimization


## Appendix: 128 Bit arithmetic

```cpp
std::pair<std::uint64_t, std::uint64_t> mul128_emu(std::uint64_t a, std::uint64_t b)
{
	using std::uint64_t;
	const uint64_t u1 = (a & 0xffffffff);
	const uint64_t v1 = (b & 0xffffffff);
	uint64_t t = (u1 * v1);
	const uint64_t w3 = (t & 0xffffffff);
	uint64_t k = (t >> 32);
	
	a >>= 32;
	t = (a * v1) + k;
	k = (t & 0xffffffff);
	uint64_t w1 = (t >> 32);
	
	b >>= 32;
	t = (u1 * b) + k;
	k = (t >> 32);
	
	const uint64_t h = (a * b) + w1 + k;
	const uint64_t l = (t << 32) + w3;
	return std::make_pair(h, l);
}

std::pair<uint64, uint64> add128(uint64 h, uint64 l, uint64 n)
{
	l += n;
	h += (l < n) ? 1 : 0;
	return std::make_pair(h, l);
}
```
