# Perfect ending for entropy coding

\newcommand{\brace}[3]{\left#1{#3}\right#2}

## Finitely odd numbers

Here we use the ideas of [Matt Timmermans](http://www3.sympatico.ca/mt0000/biacode/biacode.html) and [David A. Scott](http://bijective.dogma.net/).

In the above, we derived a method for generating an infinite stream of bits representing a real number that encodes our symbols. If the stream of symbols is finite, we want to end the stream of bits. We do this by picking a so called *finitely odd number*, these numbers:

$$
\begin{aligned}
&\mathtt{0.00000000000000}… \\
&\mathtt{0.10000000000000}… \\
&\mathtt{0.01000000000000}… \\
&\mathtt{0.11000000000000}… \\
&\mathtt{0.00100000000000}… \\
&\mathtt{0.01100000000000}… \\
&\mathtt{0.10100000000000}… \\
&\mathtt{0.11100000000000}… \\
&\mathtt{0.00010000000000}… \\
&\phantom{\mathtt{0.000100}}⋮
\end{aligned}
$$

Note that they are not sorted as ordinary real numbers, they are rather sorted in a form of [shortlex order](https://en.wikipedia.org/wiki/Shortlex_order). This is important to us because we want to encode the stream of symbols as the shortest possible bit string.

We store the first available finitely odd number that uniquely encodes the sequence of symbols. This means the number we pick must be in the final interval. But this number will also be in the final interval of all the prefixes of our current symbol sequence. To solve this problem we do the following:

* On start we set the current ending to $e = \mathtt{0.0000}…$ and reserve $E = ∅$.
* Every time we encode a symbol we:
	* Reserve the previous ending: Set $E' = E ∪ \brace\{\}{e}$.
	* Set $e'$ to the first number in $\brace[){h, l}$ and not in $E'$.
* On end we write $e$.

## Ending a bit streams

At any point in encoding, the written bit stream can be sorted in three states. Fully settled, ending on a zero and ending on a one. Let's denote these states $\mathtt{S},\mathtt{0},\mathtt{1}$ respectively. At any state we can write a zero or one. If the state is not $\mathtt{S}$ we can also add a carry.

Insert state diagram.

Besides any potential bits to output, we can also decide to trigger the carry or not. To denote endings we leave off the infinite tail of zeros and prefix a $\mathtt{1.}$ if we want to trigger the carry and $\mathtt{0.}$ if we don't. The ordered sequence of valid endings for each state are:

$\mathtt{S}$      $\mathtt{0}$      $\mathtt{1}$
----------------- ----------------- -----------------
$\mathtt{0.}$     $\mathtt{0.}$     $\mathtt{1.}$    
$\mathtt{0.1}$    $\mathtt{1.}$     $\mathtt{0.}$    
$\mathtt{0.01}$   $\mathtt{0.1}$    $\mathtt{0.1}$   
$\mathtt{0.11}$   $\mathtt{1.1}$    $\mathtt{1.1}$   
$\mathtt{0.001}$  $\mathtt{0.01}$   $\mathtt{0.01}$  
$\mathtt{0.011}$  $\mathtt{0.11}$   $\mathtt{0.11}$  
$\mathtt{0.101}$  $\mathtt{1.01}$   $\mathtt{1.01}$  
$\mathtt{0.111}$  $\mathtt{1.11}$   $\mathtt{1.11}$  
$\mathtt{0.0001}$ $\mathtt{0.001}$  $\mathtt{0.001}$ 
$\;\;⋮$           $\;\;⋮$           $\;\;⋮$         

The sequence for $\mathtt{0}$ and $\mathtt{1}⁺$ only differs in the first entry. With the exception of the first two items in $\mathtt{1}⁺$, these sequences are simply the finitely odd numbers, directly or shifted to the left once.

The endings also need to fall in the range $\brace[){h, l}$, further trimming the set of possibilities.

## Updating and pruning the reserved set

The set $E$ can currently only grow, and does so with one entry for every symbol. As we start outputting bits and carries we need to update the set. Assume the set only contains valid entries, then for each of the ten possible transitions we do the following:

1. Writing a carry, $\mathtt{0} → \mathtt{1}⁻$. Remove the items with the carry bit unset, and unset it on the remaining endings.
	
	$\mathtt{0}$       $→$  $\mathtt{1}⁻$    
	----------------- ----- -----------------
	$\mathtt{0.}$           
	$\mathtt{1.}$           $\mathtt{0.}$    
	$\mathtt{0.1}$          
	$\mathtt{1.1}$          $\mathtt{0.1}$   
	$\mathtt{0.01}$         
	$\mathtt{0.11}$         
	$\mathtt{1.01}$         $\mathtt{0.01}$  
	$\mathtt{1.11}$         $\mathtt{0.11}$  
	$\mathtt{0.001}$        
	$\;\;⋮$                 $\;\;⋮$          
	
2. Writing a carry, $\mathtt{1}⁺ → \mathtt{1}⁻$. Remove the items with the carry bit unset, and unset it on the remaining endings.
	
	$\mathtt{1}⁺$      $→$  $\mathtt{1}⁻$    
	----------------- ----- -----------------
	$\mathtt{1.}$           $\mathtt{0.}$    
	$\mathtt{0.}$           
	$\mathtt{0.1}$          
	$\mathtt{1.1}$          $\mathtt{0.1}$   
	$\mathtt{0.01}$         
	$\mathtt{0.11}$         
	$\mathtt{1.01}$         $\mathtt{0.01}$  
	$\mathtt{1.11}$         $\mathtt{0.11}$  
	$\mathtt{0.001}$        
	$\;\;⋮$                 $\;\;⋮$          
	
3. Writing a zero, $\mathtt{0} → \mathtt{0}$. Keep $\mathtt{0.}$ as $\mathtt{0.}$. Remove items with the carry bit set. Shift the remaining items one place.
	
	$\mathtt{0}$       $→$  $\mathtt{0}$    
	----------------- ----- -----------------
	$\mathtt{0.}$           $\mathtt{0.}$    
	$\mathtt{1.}$           
	$\mathtt{0.1}$          $\mathtt{1.}$   
	$\mathtt{1.1}$          
	$\mathtt{0.01}$         $\mathtt{0.1}$  
	$\mathtt{0.11}$         $\mathtt{1.1}$  
	$\mathtt{1.01}$         
	$\mathtt{1.11}$         
	$\mathtt{0.001}$        $\mathtt{0.01}$ 
	$\;\;⋮$                 $\;\;⋮$          
	
4. Writing a zero, $\mathtt{1}⁻ → \mathtt{0}$. Keep 0. as is. Shift all other items one place.
	
	$\mathtt{1}⁻$      $→$  $\mathtt{0}$    
	----------------- ----- -----------------
	$\mathtt{0.}$           $\mathtt{0.}$    
	$\mathtt{0.1}$          $\mathtt{1.}$   
	$\mathtt{0.01}$         $\mathtt{0.1}$  
	$\mathtt{0.11}$         $\mathtt{1.1}$  
	$\mathtt{0.001}$        $\mathtt{0.01}$ 
	$\mathtt{0.011}$        $\mathtt{0.11}$ 
	$\mathtt{0.101}$        $\mathtt{1.01}$ 
	$\mathtt{0.111}$        $\mathtt{1.11}$ 
	$\mathtt{0.0001}$       $\mathtt{0.001}$
	$\;\;⋮$                 $\;\;⋮$          
	
5. Writing a zero, $\mathtt{1}⁺ → \mathtt{0}$. Keep $\mathtt{0.}$ as is. Remove items with the carry bit set. Shift remaining items.
	
	$\mathtt{1}⁺$      $→$  $\mathtt{0}$    
	----------------- ----- -----------------
	$\mathtt{1.}$           
	$\mathtt{0.}$           $\mathtt{0.}$
	$\mathtt{0.1}$          $\mathtt{1.}$
	$\mathtt{1.1}$          
	$\mathtt{0.01}$         $\mathtt{0.1}$
	$\mathtt{0.11}$         $\mathtt{1.1}$
	$\mathtt{1.01}$         
	$\mathtt{1.11}$         
	$\mathtt{0.001}$        $\mathtt{0.01}$
	$\;\;⋮$                 $\;\;⋮$
	
6. Writing a one, $\mathtt{0} → \mathtt{1}⁺$. Keep $\mathtt{1.}$ as is. Keep 1. as is. Remove items starting with 1.1 or 0.0. Erase second bit.
	
	$\mathtt{0}$       $→$  $\mathtt{1}⁺$    
	----------------- ----- -----------------
	$\mathtt{0.}$           
	$\mathtt{1.}$           $\mathtt{1.}$    
	$\mathtt{0.1}$          $\mathtt{0.}$
	$\mathtt{1.1}$          
	$\mathtt{0.01}$         
	$\mathtt{0.11}$         $\mathtt{0.1}$  
	$\mathtt{1.01}$         $\mathtt{1.1}$  
	$\mathtt{1.11}$         
	$\mathtt{0.001}$        
	$\;\;⋮$                 $\;\;⋮$          
	
7. Writing a one, $\mathtt{1}⁻ → \mathtt{1}⁻$: Remove items with the second bit unset. Shift. Unset carry bit.
	
	$\mathtt{1}⁻$      $→$  $\mathtt{1}⁻$    
	----------------- ----- -----------------
	$\mathtt{0.}$           
	$\mathtt{0.1}$          $\mathtt{0.}$   
	$\mathtt{0.01}$         
	$\mathtt{0.11}$         $\mathtt{0.1}$  
	$\mathtt{0.001}$        
	$\mathtt{0.011}$        
	$\mathtt{0.101}$        $\mathtt{0.01}$ 
	$\mathtt{0.111}$        $\mathtt{0.11}$ 
	$\mathtt{0.0001}$       
	$\;\;⋮$                 $\;\;⋮$          
	
8. Writing a one, $\mathtt{1}⁺ → \mathtt{1}⁺$: Keep $\mathtt{1.}$ as is. Remove items with the first bit equal to second bit. Shift, but keep carry bits intact.
	
	$\mathtt{1}⁺$      $→$  $\mathtt{1}⁺$    
	----------------- ----- -----------------
	$\mathtt{1.}$           $\mathtt{1.}$    
	$\mathtt{0.}$           
	$\mathtt{0.1}$          $\mathtt{0.}$   
	$\mathtt{1.1}$          
	$\mathtt{0.01}$         
	$\mathtt{0.11}$         $\mathtt{0.1}$  
	$\mathtt{1.01}$         $\mathtt{1.1}$  
	$\mathtt{1.11}$         
	$\mathtt{0.001}$        
	$\;\;⋮$                 $\;\;⋮$         

We observe that the transitions preserve the order. We also note that in every transition we loose half the endings, except in $\mathrm{1}⁻→\mathrm{0}$, where we loose none.

We can also derive simple substitution rules. For example the transition $\mathrm{0} → \mathrm{01}^+$ can result in either $\mathrm{10}…$ or $\mathrm{01}$ depending on whether the ending starts with $\mathrm{1.}$ or $\mathrm{0.}$. Before the transition, these would start with $\mathrm{1.}$ and $\mathrm{0.}$ respectively.


## Implementation

\newcommand{\setb}[2]{\left\{ #1 \middle\vert #2 \right\}}

**Theorem**: The following invariant will hold:

$$
E = \setb{r ∊ S_s}{r <_s e ∧ r ∊ \brace[){h, l}}
$$

Where $S_s$ is the current sequence of endings with order $<_s$ depending on state $s$.

*Proof*: Each of the above eight transition preserves order. Pruning based on interval can only remove increasingly more items. (Proof to be completed).


  n   1   2   3   4   5   6   7   8
--- --- --- --- --- --- --- --- ---

