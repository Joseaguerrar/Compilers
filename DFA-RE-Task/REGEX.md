# Regex

12. a*b*c* -> Duda, a's pueden preceder c's?
13. a(a*)b*c*|b(b*)c*|c(c*) -> Duda, a's pueden preceder c's?
14. aa(a*)b*|ab(b*)|bb(b*)
15. (a|b)*aa(a|b)*bb(a|b)*|(a|b)*bb(a|b)*aa(a|b)*
16. (a|b)*aa(a|b)*aa(a|b)*|(a|b)*aaa(a|b)* -> Duda, aaa valido?
17. (b|c)*(a(b|c)(b|c)*)*(a|ε)
18. ε|b(a|b)*|a(ε|a|b(a|b)*|ab(a|b)*)
19. (b*ab|b*aab|b)*(aa|a|ε)
20. b*a*(bbb*aa*)*b*
21. (ab|b)*aa(ba|b)*
22. a(a|c)*b(a|c)*b(a|c)*cc
23. (a|b)*(ab(a|b)*ba|ba(a|b)*ab|aba|bab)(a|b)*
24. (a|b|c)*((aa(a|b|c)*bb(a|b|c)*cc|aa(a|b|c)*cc(a|b|c)*bb|bb(a|b|c)*aa(a|b|c)*cc|bb(a|b|c)*cc(a|b|c)*aa|cc(a|b|c)*aa(a|b|c)*bb|cc(a|b|c)*bb(a|b|c)*aa))(a|b|c)*
25. (a|c|bc)*
26. (a|b|c)(a|b|c)(a|b|c)
27. (a|b|c)(a|b|c)|(a|b|c)|ε
28. (a|b|c)(a|b|c)(a|b|c)(a|b|c)(a|b|c)*
29. b*(b*ab*ab*ab*)*
30. a*(b|c)a*(b|c)a*(b|c)a*
31. (aba|ab|ba|b)*
32. ((a|b)(a|b))*(a|b)bb((a|b)(a|b))*|((a|b)(a|b))*bb(a|b)((a|b)(a|b))*
33. ((b|c)(b|c))*(a(b|c)|(b|c)a)((b|c)(b|c))*
34. (b|a*c)*aa*b(b|a*c)*((b|a*c)*aa*b(b|a*c)*aa*b)*a*
35. (aa)*b(aa)*a(aa)*b(aa)*|a(aa)*b(aa)*b(aa)*|(aa)*b(aa)*b(aa)*a(aa)*|a(aa)*ba(aa)*ba(aa)*
36. b*(ab*ab*)*|((bb|a)*b(bb|a)*)
