library(ggplot2)
library(grid)
library(ggthemes)
theme_set(theme_light())

# Read results
results <- read.csv(file = "/home/wolf/CertifiedMaxSAT/scripts/analysis/results2021.csv", stringsAsFactors = FALSE)
colnames(results) <- c("instance", "runtime", "totalizer", "mem", "runtime_w", "proofsize", "totalizer_w", "totalizer2_w", "mem_w", "runtime_v", "mem_v", "status")
instances <- results$instance
results <- as.data.frame(lapply(results, as.numeric))
results$instance <- instances

# Safety checks
mem_limit <- 40960
time_limit <- 36000
for (row in 1:nrow(results)) {
    if (!is.na(results[row, "status"]) & results[row, "status"] == 0 & results[row, "runtime_v"] < time_limit & results[row, "mem_v"] < mem_limit) {
        print(results[row, ])
    }
}

# QMaxSATpb OOTs
for (row in 1:nrow(results)) {
    if (!is.na(results[row, "runtime_w"]) & results[row, "runtime_w"] >= 3600) {
        results[row, "runtime_w"] <- 4000
        results[row, "proofsize"] <- 0
    }
    if (!is.na(results[row, "runtime"]) & !is.na(results[row, "runtime_w"]) & results[row, "runtime_w"] < results[row, "runtime"]) {
        print(results[row, ])
    }
}

# QMaxSATpb OOMs
for (row in 1:nrow(results)) {
    if (!is.na(results[row, "mem_w"]) & results[row, "mem_w"] >= 32768) {
        results[row, "runtime_w"] <- 8500
        results[row, "proofsize"] <- 0
    }
}

# VeriPB OOTs
for (row in 1:nrow(results)) {
    if (!is.na(results[row, "runtime_v"]) & results[row, "runtime_v"] >= 36000) {
        results[row, "runtime_v"] <- 46000
    }
}

# VeriPB OOMs
for (row in 1:nrow(results)) {
    if (!is.na(results[row, "mem_v"]) & results[row, "mem_v"] >= 40960) {
        print(results[row, "instance"])
        print(results[row, "proofsize"])
        results[row, "runtime_v"] <- 99000
    }
}


#--------------------------------
# PLOTS
#--------------------------------

# TYPE1
no_NAs <- results[!is.na(results$runtime) & results$runtime < 3600 & !is.na(results$mem) & results$mem < 32768, ]

ggplot(no_NAs, aes(x = runtime_w, y = runtime, color = log10(mem_w + 1))) +
    # ggplot(no_NAs, aes(x = runtime_w, y = runtime, color = log10((proofsize / 10^3) + 1))) +
    geom_point() +
    scale_x_log10(breaks = c(1, 10, 100, 1000, 10000)) +
    scale_y_log10(breaks = c(1, 10, 100, 1000, 10000)) +
    scale_color_continuous(breaks = c(1, 2, 3, 4, 5, 6), labels = c("10MB", "100MB", "1GB", "10GB", "100GB")) +
    # scale_color_continuous(breaks = c(3, 6, 9), labels = c("1MB", "1GB", "1TB")) +
    coord_fixed(ratio = 1) +
    geom_vline(xintercept = 4000, linetype = "dashed") +
    geom_vline(xintercept = 8500, linetype = "dashed") +
    geom_abline(slope = 1, intercept = 0, linetype = "dashed") +
    labs(color = "Memory used") +
    xlab("QMaxSATpb (time in s)") +
    ylab("QMaxSAT (time in s)") +
    coord_cartesian(xlim = c(0.1, 10000), ylim = c(0.1, 10000)) +
    annotate(
        geom = "text",
        label = "OoT",
        x = 4250,
        y = 0.081,
        angle = 90,
        vjust = 1,
        size = 2
    ) +
    annotate(
        geom = "text",
        label = "OoM",
        x = 9100,
        y = 0.081,
        angle = 90,
        vjust = 1,
        size = 2
    )
ggsave("./scripts/analysis/without_vs_with.pdf", device = "pdf", width = 14, height = 12, units = "cm")

# TYPE2
no_NAs2 <- results[!is.na(results$runtime) & results$runtime < 3600 & !is.na(results$mem) & results$mem < 32768, ]

ggplot(no_NAs2, aes(x = runtime_v, y = runtime_w, color = log10(mem_v + 1))) +
    # ggplot(no_NAs, aes(x = runtime_w, y = runtime, color = log10((proofsize / 10^3) + 1))) +
    geom_point() +
    scale_x_log10(breaks = c(1, 10, 100, 1000, 10000)) +
    scale_y_log10(breaks = c(1, 10, 100, 1000, 10000)) +
    scale_color_continuous(breaks = c(1, 2, 3, 4, 5), labels = c("10MB", "100MB", "1GB", "10GB", "100GB")) +
    # scale_color_continuous(breaks = c(3, 6, 9), labels = c("1MB", "1GB", "1TB")) +
    coord_fixed(ratio = 1) +
    geom_vline(xintercept = 46000, linetype = "dashed") +
    geom_vline(xintercept = 99000, linetype = "dashed") +
    geom_abline(slope = 1, intercept = 0, linetype = "dashed") +
    xlab("VeriPB (time in s)") +
    ylab("QMaxSATpb (time in s)") +
    labs(color = "Memory used") +
    coord_cartesian(xlim = c(0.1, 100000), ylim = c(0.1, 10000)) +
    annotate(
        geom = "text",
        label = "OoT",
        x = 49000,
        y = 0.081,
        angle = 90,
        vjust = 1,
        size = 2
    ) +
    annotate(
        geom = "text",
        label = "OoM",
        x = 105000,
        y = 0.081,
        angle = 90,
        vjust = 1,
        size = 2
    )
ggsave("./scripts/analysis/solving_vs_verification.pdf", device = "pdf", width = 18, height = 12, units = "cm")

# Total solved instances
runtime <- sort(results[!is.na(results$runtime) & results$runtime < 3600, "runtime"])
runtime_w <- sort(results[!is.na(results$runtime_w) & results$runtime_w < 3600, "runtime_w"])
combined <- rbind(setNames(data.frame(runtime, cumsum(rep(1, length(runtime))), rep("QMaxSAT", length(runtime))), c("runtime", "solved_in_time", "solver")), setNames(data.frame(runtime_w, cumsum(rep(1, length(runtime_w))), rep("QMaxSATpb", length(runtime_w))), c("runtime", "solved_in_time", "solver")))

p <- ggplot(data = combined) +
    geom_line(aes(x = solved_in_time, y = runtime, colour = solver)) +
    geom_point(aes(x = solved_in_time, y = runtime, colour = solver), shape = 24) +
    labs(x = "Number of solved instances", y = "Time in seconds") +
    scale_colour_manual("Configuration", values = c("red", "blue")) +
    scale_y_continuous(sec.axis = dup_axis(name = NULL, labels = NULL)) +
    scale_x_continuous(sec.axis = dup_axis(name = NULL, labels = NULL)) +
    theme(
        legend.position = c(0.1, 0.9), panel.grid.major = element_blank(), panel.grid.minor = element_blank(),
        panel.background = element_blank(), axis.line = element_line(colour = "black"),
        axis.ticks.length = unit(-0.2, "cm")
    )
ggsave("./scripts/analysis/total.pdf", device = "pdf", width = 30, height = 20, units = "cm")