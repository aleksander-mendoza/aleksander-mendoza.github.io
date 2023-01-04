
theory Test
  imports Main
begin

  
inductive_set even :: "nat set" where 
zero[intro!]: "Zero \<in> even" |
step[intro!]: "n \<in> even \<Longrightarrow> (Suc (Suc n)) \<in> even"

lemma two_times_even[intro!]: "2*k \<in> even" 
  apply (induct_tac k)
   apply auto 
  done



no_notation Nil ("[]") and Cons (infixr "#" 65) and append (infixr "@" 65)
hide_const rev
hide_type list

lemma "\<not> surj(f :: 'a \<Rightarrow> 'a set)" 
proof
  assume 0: "surj f"
  from 0 have 1: "\<forall>A. \<exists>a. A = f a" by(simp add: surj_def) 
  from 1 have 2: "\<exists>a. {x. x \<notin> f x} = f a" by blast
  from 2 show "False" by blast
qed

fun add :: "nat \<Rightarrow> nat \<Rightarrow> nat" where "add 0 n = n" |
"add (Suc m) n = Suc(add m n)"

inductive_set Fin :: "'a set \<Rightarrow> 'a set set" for A :: "'a set" where
empty: "{} \<in> Fin A" |
insert: "a \<in> A \<Longrightarrow> B \<in> Fin A \<Longrightarrow> insert a B \<in> Fin A"

end