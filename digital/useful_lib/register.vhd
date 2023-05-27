use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.std_logic_unsigned.all; 

entity Registro is
	port 
	(
		D	: in	std_logic_vector(7 downto 0);
		Q 	: out  	std_logic_vector(7 downto 0);
		En	: in 	std_logic;
		Clk 	: in 	std_logic;
		Reset 	: in 	std_logic		
	);

end Registro;

architecture behav of Registro is
begin

	process(clk)
	begin
		if (clk='1' and clk'event) then
			if (reset ='1') then	
				Q <= (others =>'0');
			elsif (En = '1') then
				Q <= D;
			end if;
		end if;
	end process;
end behav;