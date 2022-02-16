---
title: "Preliminary Dominance Experiments"
output:
  pdf_document: default
  html_notebook: default
---

```{r setup, include=FALSE}
knitr::opts_chunk$set(echo = TRUE)

library(dplyr)
library(tidyr)
library(ggplot2)
library(ggthemes)
library(tibble)
library(xtable)
library(stringr)
theme_set(theme_light())


sizes = NULL
times = NULL

paperPlots = "/home/stephan/phd/projects/prooflogging/dominance-symmetry-veripb/aaai-22/plots/"

batches = Sys.glob("data/*/")
for (folder in batches) {
  batch = sub("data/(.*)/", "\\1", folder)
  batch = gsub("_", ".", batch)
  batch = paste("b.", batch, sep = "")
  print(batch)
  
  for (file in Sys.glob(paste(folder, "*sizes*", sep = ""))) {
    df <- read.csv(file, sep=";", strip.white = TRUE)
    df <- df %>% mutate("batch" = batch)
    if (is.null(sizes)) {
      sizes <- df
    } else {
      sizes <- bind_rows(sizes, df)
    }
  }
  
  for (file in Sys.glob(paste(folder, "*timings*", sep = ""))) {
    df <- read.csv(file, sep=";", strip.white = TRUE)
    df <- df %>% mutate("batch" = batch, "Time" = as.character(Time))
    if (is.null(times)) {
      times <- df
    } else {
      times <- bind_rows(times, df)
    }
  }
}

# batch b.21.05.18 contains rerun of instances that failed in b.21.05.10
# so let us create a merged batch
from = "b.21.05.10"
to = "b.21.05.18"
result_batch = "b.21.05.18+"

df <- times %>% filter(batch %in% c(from, to)) %>%
  arrange(desc(batch)) %>%
  distinct(File, Tool, Configuration, .keep_all = TRUE)

times <- bind_rows(times, df %>% mutate("batch" = result_batch))

df <- sizes %>% filter(batch %in% c(from, to)) %>%
  arrange(desc(batch)) %>%
  distinct(File, Tool, Configuration, .keep_all = TRUE)

sizes <- bind_rows(sizes, df %>% mutate("batch" = result_batch))

# batch b.21.05.30 contains rerun of instances that failed in b.21.05.23
# so let us create a merged batch
from = "b.21.05.30"
to = "b.21.05.23"
result_batch = "b.21.05.30+"

df <- times %>% filter(batch %in% c(from, to)) %>%
  arrange(desc(batch)) %>%
  distinct(File, Tool, Configuration, .keep_all = TRUE)

times <- bind_rows(times, df %>% mutate("batch" = result_batch))

df <- sizes %>% filter(batch %in% c(from, to)) %>%
  arrange(desc(batch)) %>%
  distinct(File, Tool, Configuration, .keep_all = TRUE)

sizes <- bind_rows(sizes, df %>% mutate("batch" = result_batch))


mixed = c("b.21.09.08", "b.21.09.04") #, "b.21.09.09"
result_batch = "b.21.09.04a"

df <- times %>% filter(batch %in% mixed) %>%
  arrange(desc(batch)) %>%
  distinct(File, Tool, Configuration, .keep_all = TRUE)

times <- bind_rows(times, df %>% mutate("batch" = result_batch))

df <- sizes %>% filter(batch %in% c(from, to)) %>%
  arrange(desc(batch)) %>%
  distinct(File, Tool, Configuration, .keep_all = TRUE)

sizes <- bind_rows(sizes, df %>% mutate("batch" = result_batch))

mixed = c("b.21.09.08", "b.21.09.04", "b.21.09.09")
result_batch = "b.21.09.04b"

df <- times %>% filter(batch %in% mixed) %>%
  arrange(desc(batch)) %>%
  distinct(File, Tool, Configuration, .keep_all = TRUE)

times <- bind_rows(times, df %>% mutate("batch" = result_batch))

df <- sizes %>% filter(batch %in% c(from, to)) %>%
  arrange(desc(batch)) %>%
  distinct(File, Tool, Configuration, .keep_all = TRUE)

sizes <- bind_rows(sizes, df %>% mutate("batch" = result_batch))


times <- times %>% mutate("Time" = pmax(0.1, as.numeric(Time))) %>%
  distinct(File, Tool, Configuration, batch, .keep_all = TRUE)
times <- times %>% mutate("Time" = as.numeric(Time))

sizes <- sizes %>% mutate(
  what = replace_na(gsub("(.*)\\.([^\\.]*)\\.pbp", "\\2", str_match(File, ".*.pbp")),"instance"),
  instance = gsub("(.*\\.cnf).*", "\\1", File))
```


```{r}
times %>% filter(Tool == "BreakID", Outcode != 0)
```


```{r}
write.csv(sizes %>% filter(batch == "b.21.09.04b") %>% select(-batch), "sizes.csv")
write.csv(times %>% filter(batch == "b.21.09.04b") %>% select(-batch), "times.csv")

write.csv(times %>% filter(batch %in% c("b.21.09.11", "b.21.09.12")) %>% group_by(Tool, Configuration, File) %>% slice_max(order_by = batch) %>% select(-batch), "rerun.csv")

times %>% filter(batch == "b.21.09.12") %>% filter(Outcode == 1)
```

```{r}

```


# Time variation in douplicate run.

Note: Turn off distinct rows above before using.

```{r}
suppressWarnings(
df <- times %>% filter(batch == "b.21.08.18") %>%
  select(File, Tool, Configuration, Time) %>%
  mutate(Time = as.numeric(as.character(Time))) %>%
  filter(Time > 10) %>%
  group_by(File, Tool, Configuration) %>%
  mutate(rank = rank(Time, ties.method = "random")) %>%
  pivot_wider(names_from = rank, values_from = Time) %>%
  mutate(avg = (`1`+`2`)/2, diff = max(`1`,`2`) - min(`1`,`2`)) %>%
  filter(!is.na(avg))
)

df <- df %>% mutate(percentage = diff / avg * 100)

ggplot(df, aes(x = percentage)) + geom_boxplot() +
  scale_x_continuous(breaks = 0:20 * 10) +
  scale_y_discrete()
ggsave("variation.pdf", device="pdf", width = 20 , height = 5, units = "cm")
```


```{r}
names(times)
times %>%filter(Tool == "Kissat", Configuration == "Unbroken", !is.na(Time), Outcode == 20, Time > 10)%>% select(Time, File) %>% arrange(Time)

```
```{r}
times %>% select(batch) %>% unique() %>% arrange(batch)
```


```{r}
times %>% filter(Tool == "VeriPB") %>% select("Configuration") %>% unique()
times %>% select(Tool) %>% unique()

to = 5000
df <- times %>% 
  select(File, Tool, Configuration, Time, batch) %>%
  filter(Tool == "VeriPB", Configuration == "OnlySymBroken") %>%
  #filter(Tool == "VeriPB", Configuration == "UnBroken") %>%
  #filter(Tool == "VeriPB", Configuration == "Broken") %>%
  pivot_wider(names_from = batch, values_from = Time)

ggplot(df, aes(x = b.21.06.20, y = b.21.09.04)) +
        geom_point(shape = 4) +
        scale_x_log10(limits = c(0.1,10000),breaks = c(1,10,100,1000)) + 
        scale_y_log10(limits = c(0.1,10000),breaks = c(1,10,100,1000)) +
        coord_fixed(ratio = 1) +
        geom_vline(xintercept = to, linetype="dashed") +
        geom_hline(yintercept = to, linetype="dashed") +
        geom_abline(slope = 1, intercept = 0, linetype="dashed")
        # xlab("RoundingSAT (time in s)") + ylab("CryptoMiniSAT (time in s)") + labs(color = "Hash Function")

# ggsave("cryptominisat_vs_roundingsat.pdf", device="pdf", width = 11 , height = 7.5, units = "cm")
```


```{r}
to = 5000

df <- sizes %>% 
  select(File, Proofsize, batch) %>%
  pivot_wider(names_from = batch, values_from = Proofsize)

ggplot(df, aes(x = b.21.04.27, y = b.21.04.26)) +
        geom_point(shape = 4) +
        #scale_x_log10(limits = c(0.1,10000),breaks = c(1,10,100,1000)) + 
        #scale_y_log10(limits = c(0.1,10000),breaks = c(1,10,100,1000)) +
        scale_x_log10() + 
        scale_y_log10() +
        coord_fixed(ratio = 1) +
        geom_vline(xintercept = to, linetype="dashed") +
        geom_hline(yintercept = to, linetype="dashed") +
        geom_abline(slope = 1, intercept = 0, linetype="dashed")
        # xlab("RoundingSAT (time in s)") + ylab("CryptoMiniSAT (time in s)") + labs(color = "Hash Function")

# ggsave("cryptominisat_vs_roundingsat.pdf", device="pdf", width = 11 , height = 7.5, units = "cm")
```

```{r}
times %>% filter(File == "benchmarks/2017/NoLimits/mp1-blockpuzzle_9x9_s1_free9.cnf") %>% select(batch, Configuration, Tool, Time, Memory)
```

```{r}
to = 20000
df <- times %>% 
  filter(batch == "b.21.05.22", 
         #Tool != "VeriPB" | Outcode == 0
         #(Tool == "VeriPB" & Configuration == "Broken" & Outcode == 0) |
         #(Tool == "BreakId" & Configuration == "WithLog") |
         #(Tool == "Kissat" & Configuration == "Broken")
         ) %>%
    select(File, Tool, Configuration, Time, Outcode, Memory) %>%
  pivot_wider(names_from = c(Tool, Configuration), values_from = c(Time, Outcode, Memory))

df %>% filter()

df <- df %>% filter(!is.na(Time_Kissat_Broken)) %>%
  mutate(state = ifelse(is.na(Time_VeriPB_Broken), "time_out", ifelse(Outcode_VeriPB_Broken != 0, "mem_out", "ok"))) %>%
  mutate(Time_VeriPB_Broken = ifelse(state == "time_out", 2*to, ifelse(state == "mem_out", 3*to, Time_VeriPB_Broken)))

df %>% filter(is.na(Time_VeriPB_Broken))

df %>% filter(is.na(state)) %>% select(Outcode_Kissat_Broken, Time_VeriPB_Broken) %>% distinct()

df %>% group_by(state) %>% summarize(count = n())

names(df)
# ggplot(df, aes(x = Time_VeriPB_Broken, y = Time_Kissat_Broken + Time_BreakId_WithLog, color = is.na(Time_Kissat_Unbroken))) +
# ggplot(df, aes(x = Time_Kissat_Broken, y = Time_Kissat_Unbroken, color = log(Time_Kissat_Unbroken / Time_Kissat_Broken))) +
ggplot(df, aes(x = Time_Kissat_Broken, y = Time_Kissat_BrokenNoLog, color = log(Time_Kissat_Unbroken / Time_Kissat_Broken))) +
# ggplot(df, aes(x = Memory_VeriPB_Broken, y = Memory_Kissat_Broken + Memory_BreakId_WithLog)) +
        geom_point(shape = 4) +
        scale_x_log10(limits = c(0.1,3*to),breaks = c(1,10,100,1000,10000)) + 
        scale_y_log10(limits = c(0.1,3*to),breaks = c(1,10,100,1000,10000)) +
        #scale_x_log10() +
        #scale_y_log10() +
        coord_fixed(ratio = 1) +
        geom_vline(xintercept = to, linetype="dashed") +
        geom_hline(yintercept = to, linetype="dashed") +
        geom_abline(slope = 1, intercept = 0, linetype="dashed") +
        labs(color = "Unsolved Without Breaking")
        # xlab("RoundingSAT (time in s)") + ylab("CryptoMiniSAT (time in s)") + 


```

```{r}
to = 5000
#batch_name = "b.21.07.24"
#batch_name = "b.21.06.20"
#batch_name = "b.21.09.04b"
batch_name = "b.21.11.10"

data <- times %>% 
  filter(batch == batch_name) %>%
  select(File, Tool, Configuration, Time, Outcode, Memory) %>%
  pivot_wider(names_from = c(Tool, Configuration), values_from = c(Time, Outcode, Memory))


data <- left_join(data,
                 sizes %>% filter(batch == batch_name, Tool == "BreakId", Configuration == "WithLog") %>% select(File, Proofsize, DominanceLines),
                 by = c("File" = "File")
    )

data <- data %>% filter(!is.na(Proofsize) | (!is.na(Time_BreakId_NoLog)))

data %>% filter( is.na(Time_BreakId_WithLog), !is.na(Time_BreakId_NoLog) )  %>% select(Time_Kissat_UnbrokenNoLog, Time_Kissat_BrokenNoLog
                                                                                       )

inner_join(
  data %>% filter( is.na(Time_Kissat_BrokenNoLog), !is.na(Time_Kissat_UnbrokenNoLog) ),
  data %>% filter( is.na(Time_BreakId_WithLog), !is.na(Time_BreakId_NoLog) ))

data %>% filter( is.na(Time_BreakId_WithLog), !is.na(Time_BreakId_NoLog) )

df <- data

df %>% filter(is.na(Time_Kissat_Unbroken), !is.na(Time_Kissat_BrokenNoLog)) %>% summarize(all = n(), verified = sum(!is.na(Time_VeriPB_Broken)))

df %>% filter(!is.na(Time_BreakId_NoLog), !is.na(Time_BreakId_WithLog)) %>% mutate("speedup" = Time_BreakId_WithLog / Time_BreakId_NoLog) %>% pull(speedup) %>% quantile(probs = c(0.25, 0.50, 0.75, 0.95, 1))

df %>% summarize(count = n(), solved = sum(!is.na(Time_BreakId_WithLog)), timeout = sum(is.na(Time_BreakId_WithLog)))

df <- df %>% mutate(Time_BreakId_WithLog = ifelse(is.na(Time_BreakId_WithLog), 2*to, Time_BreakId_WithLog))

df %>% filter(!is.na(Time_BreakId_NoLog), is.na(Time_BreakId_WithLog))





ggplot(df, aes(x = Time_BreakId_WithLog, y = Time_BreakId_NoLog, color = log10(Proofsize + 1))) +
        geom_point(shape = 4) +
        scale_x_log10(limits = c(0.1,10000),breaks = c(1,10,100,1000,10000)) + 
        scale_y_log10(limits = c(0.1,10000),breaks = c(1,10,100,1000,10000)) +
        scale_color_continuous(breaks = c(3, 6, 9), labels = c("1MB", "1GB", "1TB")) +
        #scale_x_log10() +
        #scale_y_log10() +
        coord_fixed(ratio = 1) +
        geom_vline(xintercept = to, linetype="dashed") +
        #geom_hline(yintercept = to, linetype="dashed") +
        geom_abline(slope = 1, intercept = 0, linetype="dashed") +
        labs(color = "Proofsize") +
        xlab("BreakID + Proof Logging (time in s)") + ylab("BreakID (time in s)") + 

ggsave(paste("plots/", batch_name, "breaking_log_vs_no_log.pdf", sep = ""), device="pdf", width = 20 , height = 20, units = "cm")
ggsave(paste(paperPlots, "breaking_log_vs_no_log.pdf", sep = ""), device="pdf", width = 10, height = 8, units = "cm")
```

```{r}
to = 100000

data <- data %>% mutate("RequiresSym" = ifelse(is.na(Time_Kissat_Unbroken), ifelse(is.na(Time_Kissat_Broken), "unsolved", "yes"), "no") ) 

df <- data %>% filter(!is.na(Time_BreakId_WithLog))

df %>% filter(!Outcode_VeriPB_OnlySymBroken %in% c(0, 1, 4), !is.na(Outcode_VeriPB_OnlySymBroken)) %>% pull(File)

df %>% filter(!Outcode_VeriPB_OnlySymBroken %in% c(0, 1, 4), !is.na(Outcode_VeriPB_OnlySymBroken)) %>% arrange(Time_VeriPB_OnlySymBroken)

df <- df %>%
  mutate(state = ifelse(is.na(Time_VeriPB_OnlySymBroken), "time_out", ifelse(Outcode_VeriPB_OnlySymBroken != 0, "mem_out", "ok"))) %>%
  mutate(Time_VeriPB_OnlySymBroken = ifelse(state == "time_out", 2*to, ifelse(state == "mem_out", 3*to, Time_VeriPB_OnlySymBroken)))

bind_rows(
  df %>% group_by(state) %>% summarize(count = n()) %>% select(count, state),
  df %>% summarize(count = n()) %>% mutate("state" = "all") ) 

df %>% filter(state == "ok") %>% mutate("speedup" = Time_VeriPB_OnlySymBroken / Time_BreakId_WithLog) %>% pull(speedup) %>% quantile(probs = c(0.25, 0.50, 0.75, 0.95))

ggplot(df, aes(x = Time_VeriPB_OnlySymBroken, y = Time_BreakId_WithLog, color = RequiresSym, shape = RequiresSym)) +
        geom_point() +
        scale_x_log10(limits = c(0.1,3*to),breaks = c(1,10,100,1000,10000)) + 
        scale_y_log10(limits = c(0.1,10000),breaks = c(1,10,100,1000,10000)) +
        #scale_x_log10() +
        #scale_y_log10() +
        coord_fixed(ratio = 1) +
        geom_vline(xintercept = to, linetype="dashed") +
        #geom_hline(yintercept = to, linetype="dashed") +
        geom_abline(slope = 1, intercept = 0, linetype="dashed") +
        labs(color = "Requires Breaking", shape = "Requires Breaking") +
        theme(legend.position="top") +
        xlab("VeriPB (time in s)") + ylab("BreakID + Proof Logging (time in s)") +
        scale_color_manual(values = c("#d95f02", "#7570b3","#1b9e77")) +
        scale_shape_manual(values = c(17,4,16))

ggsave(paste("plots/", batch_name, "break_vs_verification.pdf", sep = ""), device="pdf", width = 20 , height = 20, units = "cm")
ggsave(paste(paperPlots, "break_vs_verification.pdf", sep = ""), device="pdf", width = 10 , height = 9, units = "cm")
```

```{r}
to = 100000

df <- data %>% filter(!is.na(Time_Kissat_Broken)) %>%
  mutate(state = ifelse(is.na(Time_VeriPB_Broken), "time_out", ifelse(Outcode_VeriPB_Broken != 0, "mem_out", "ok"))) %>%
  mutate(Time_VeriPB_Broken = ifelse(state == "time_out", 2*to, ifelse(state == "mem_out", 3*to, Time_VeriPB_Broken)))

bind_rows(
  df %>% group_by(state) %>% summarize(count = n()) %>% select(count, state),
  df %>% summarize(count = n()) %>% mutate("state" = "all") )

df %>% filter(state == "ok") %>% mutate("speedup" = (Time_VeriPB_Broken / Time_Kissat_Broken + Time_BreakId_WithLog)) %>% pull(speedup) %>% quantile(probs = c(0.25, 0.50, 0.75))

df %>% filter(is.na(Time_Kissat_UnbrokenNoLog), !is.na(Time_Kissat_BrokenNoLog)) %>% summarize(requireingSymmetryBreaking = n(), verified = sum(!is.na(Time_VeriPB_Broken)))

df %>% filter(!is.na(Time_Kissat_UnbrokenNoLog), is.na(Time_Kissat_BrokenNoLog))

df %>% filter(!is.na(Time_BreakId_NoLog), is.na(Time_BreakId_WithLog))

ggplot(df, aes(x = Time_VeriPB_Broken, y = Time_Kissat_Broken + Time_BreakId_WithLog, color = RequiresSym, shape = RequiresSym)) +
        geom_point() +
        scale_x_log10(limits = c(0.1,3*to),breaks = c(1,10,100,1000,10000)) + 
        scale_y_log10(limits = c(0.1,10000),breaks = c(1,10,100,1000,10000)) +
        #scale_x_log10() +
        #scale_y_log10() +
        coord_fixed(ratio = 1) +
        geom_vline(xintercept = to, linetype="dashed") +
        #geom_hline(yintercept = to, linetype="dashed") +
        geom_abline(slope = 1, intercept = 0, linetype="dashed") +
        labs(color = "Requires Breaking", shape = "Requires Breaking") +
        theme(legend.position="top") +
        xlab("VeriPB (time in s)") + ylab("BreakID + Kissat (time in s)") +
        scale_color_manual(values = c("#d95f02","#1b9e77")) +
        scale_shape_manual(values = c(17,16))
        

ggsave(paste("plots/", batch_name, "solve_vs_verification.pdf", sep = ""), device="pdf", width = 20 , height = 20, units = "cm")

ggsave(paste(paperPlots, "solve_vs_verification.pdf", sep = ""), device="pdf", width = 10 , height = 9, units = "cm")
```


```{r}
names(times)

batch_name

times %>% filter(batch == batch_name, Tool == "VeriPB", Outcode == 2) %>% select(File) %>% distinct() %>% pull()
```




```{r}
names(times)
times %>% filter(Tool == "VeriPB") %>% select(Configuration) %>% unique()

sizes %>% filter(what == "instance")

sizes%>% filter(batch == batch_name)

df <- inner_join(
sizes %>%
  filter(batch == batch_name) %>%
  select(File, Tool, Configuration, Proofsize) %>%
  pivot_wider(names_from = c(Tool, Configuration), values_from = Proofsize, names_prefix="Proofsize_")
,
times %>% 
  filter(batch == batch_name) %>%
  select(File, Tool, Configuration, Time, Outcode, Memory) %>%
  pivot_wider(names_from = c(Tool, Configuration), values_from = c(Time, Outcode, Memory))
, by = ("File" = "File")
)


ggplot(df, aes(x = Proofsize_Kissat_Unbroken, y = Time_VeriPB_UnBroken)) +
        geom_point(shape = 4) +
        #scale_x_log10(limits = c(0.1,3*to),breaks = c(1,10,100,1000,10000)) + 
        #scale_y_log10(limits = c(0.1,3*to),breaks = c(1,10,100,1000,10000)) +
        scale_x_log10() +
        scale_y_log10() +
        #coord_fixed(ratio = 1) +
        #geom_vline(xintercept = to, linetype="dashed") +
        #geom_hline(yintercept = to, linetype="dashed") +
        geom_abline(slope = 1, intercept = 0, linetype="dashed")
        # labs(color = "Unsolved Without Breaking")
        # xlab("RoundingSAT (time in s)") + ylab("CryptoMiniSAT (time in s)") + 


names(df)

ggplot(df, aes(x = Proofsize_BreakId_WithLog + Proofsize_Kissat_Broken, y = Time_VeriPB_Broken)) +
        geom_point(shape = 4) +
        scale_x_log10() +
        scale_y_log10() +
        geom_abline(slope = 1, intercept = 0, linetype="dashed")

ggplot(df, aes(x = Proofsize_BreakId_WithLog, y = Time_VeriPB_OnlySymBroken)) +
        geom_point(shape = 4) +
        scale_x_log10() +
        scale_y_log10() +
        geom_abline(slope = 1, intercept = 0, linetype="dashed")

```


Sanity Checks
=============

```{r}
#selectedBatch = "b.21.05.18+"
selectedBatch = "b.21.11.10"

df <- times %>% 
  filter(batch == selectedBatch) %>%
    select(File, Tool, Configuration, Time, Outcode) %>%
  pivot_wider(names_from = c(Tool, Configuration), values_from = c(Time, Outcode))


#Hard instances without symmetry breaking

df %>% filter(is.na(Time_Kissat_Unbroken), !is.na(Time_Kissat_Broken)) %>% 
  select(File, Time_Kissat_Unbroken, Time_Kissat_Broken, Time_VeriPB_Broken, Time_BreakId_WithLog) %>%
  arrange(Time_VeriPB_Broken)

# Kissat without proof logging failed

df %>% group_by(Outcode_Kissat_BrokenNoLog, Time_Kissat_BrokenNoLog) %>% summarize(count = n())

times %>% filter(batch == selectedBatch, Tool == "Kissat", Configuration == "BrokenNoLog")
times %>% filter(File == "benchmarks/2020/planning/UNSAT_ME_seq-sat_Thoughtful_target-typed-25.pddl_39.cnf", batch == "b.21.05.10", Tool == "Kissat") %>% select(Configuration, Command)

# VeriPB bad return codes
times %>% filter(batch == selectedBatch, Tool == "VeriPB") %>% group_by(Outcode) %>% summarize(minMem = min(Memory), count = n())
# Return code 127 is command not found but seems to be clearly caused by memout
# Return code 4 is internal error and seems to mostly happen due to memory problems
times %>% filter(batch == selectedBatch, Tool == "VeriPB", Outcode == 4, Memory < 7000000) %>% arrange(desc(Memory))


# Return code 4 is parse error and only happened when Kissat timed out:
df %>% filter(Outcode_VeriPB_UnBroken == 4) %>% summarize( na = sum(is.na(Outcode_Kissat_Unbroken)), count = n())
df %>% filter(Outcode_VeriPB_Broken == 4) %>% summarize( na = sum(is.na(Outcode_Kissat_Broken)), count = n())
df %>% filter(Outcode_VeriPB_OnlySymBroken == 4)

df %>% filter(Outcode_VeriPB_UnBroken == 1) %>% summarize( na = sum(is.na(Outcode_Kissat_Unbroken)), count = n())
df %>% filter(Outcode_VeriPB_Broken == 1) %>% summarize( na = sum(is.na(Outcode_Kissat_Broken)), count = n())
df %>% filter(Outcode_VeriPB_OnlySymBroken == 1) %>% summarize( na = sum(is.na(Outcode_Kissat_Broken)), count = n())


df %>% filter(Outcode_VeriPB_UnBroken == 2)

names(df)

times %>% filter(batch == selectedBatch, Tool == "VeriPB", Outcode == 2, Memory < 7000000) %>% arrange(desc(Memory))

# Parse Error
times %>% filter(batch == selectedBatch, Tool == "VeriPB", Outcode == 4) %>% group_by(File) %>% summarize(n = n())




times %>% filter(batch == selectedBatch, Tool == "VeriPB", File == "benchmarks/2016/app16/Ledoux/sncf_model_ixl_bmc_depth_07.cnf")



# Return code 1 is failed to verify and therefore the most worry some
times %>% filter(batch == selectedBatch, Tool == "VeriPB", Outcode == 1, Memory < 7000000) %>% arrange(Time)
df %>% filter(Outcode_VeriPB_Broken == 1, !is.na(Outcode_Kissat_Broken)) %>% arrange(Time_BreakId_WithLog) %>% select(Outcode_Kissat_Broken, Time_VeriPB_Broken, File) %>% pull()


#VeriPB faster than breaking and solving???
df %>% filter(Time_VeriPB_Broken < Time_Kissat_Broken + Time_BreakId_WithLog, Outcode_VeriPB_Broken == 0) %>% select(Time_VeriPB_Broken, Time_Kissat_Broken, Time_BreakId_WithLog, Outcode_VeriPB_Broken, File)

sizes %>% filter(File == "benchmarks/2016/crafted16/Elffers/tseitingrid4x190_shuffled.cnf")


times %>% filter(batch == selectedBatch) %>% select(Tool, Configuration, File, Outcode, Time) %>% pivot_wider(names_from = c(Tool, Configuration), values_from = c(Outcode, Time)) %>% select(Time_Kissat_Broken, Outcode_VeriPB_Broken) %>% filter(Outcode_VeriPB_Broken == 4)
```



```{r}

#find broken files
times %>% filter(batch == "b.21.05.10", Tool == "VeriPB", Outcode != 0, !is.na(Outcode)) %>% select(Configuration, Time, Outcode, File, Command) # %>% pull(File)

  times %>% filter(batch == "b.21.05.10", File == "benchmarks/2020/main/harder-php-025-024.sat05-1191.reshuffled-07.cnf") %>% select(Tool, Configuration, Time, Outcode, File, Command)
```


Extract instances where symmetrie breaking did become slower:

```{r}
df <- times %>% 
  select(File, Tool, Configuration, Time, batch) %>%
  filter(Tool == "VeriPB", Configuration == "OnlySymBroken") %>%
  pivot_wider(names_from = batch, values_from = Time)

df %>% filter(b.21.06.20 * 10 < b.21.07.24, b.21.07.24 < 50, b.21.06.20 >= 1) %>% mutate(ratioNew = b.21.08.18 / b.21.06.20) %>% filter(ratioNew > 4) %>% select(File, b.21.08.18, b.21.06.20, b.21.07.24)

```

```{r}
full_join(data1, data2, by = "File") %>% select(Time_Kissat_BrokenNoLog.x, Time_Kissat_BrokenNoLog.y)

```

