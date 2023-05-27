library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
-- per dare un senso agli operatori +1 e -1:
use ieee.std_logic_unsigned.all; 


entity RegistroIncDec8bit is

	port 
	(
		Q : out  std_logic_vector(7 downto 0);
		D : in  std_logic_vector(7 downto 0);
		Set: in std_logic;
		Inc: in std_logic;
		Dec: in std_logic;
		Clk : in std_logic;
		Reset : in std_logic
	);

end RegistroIncDec8bit;
architecture behav of RegistroIncDec8bit is
signal currVal  :  std_logic_vector(7 downto 0);
signal nextVal :  std_logic_vector(7 downto 0);
signal IncDecSet :  std_logic_vector(2 downto 0); -- segnale ausiliario
begin

	process(clk)
	begin
		if (clk='1' and clk'event) then
			if (reset ='1') then	
				currVal <= (others =>'0');
			else
				currVal <= nextVal;
			end if;
		end if;
	end process;
	
	-- selezione del prossimo valore interno
	IncDecSet <= Inc & Dec & Set;
	with IncDecSet select
		nextVal <= D		when "001",
			   currVal + 1  when "100",
			   currVal - 1  when "010",
			   currVal		when others;  -- others assures combinatorial synthesis	
			   				  
	Q <= currVal;

end behav;